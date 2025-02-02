#include <shell_launcher.h>

#include <pthread.h>
#include <giomm/init.h>
#include <giomm/appinfo.h>
#include <mir_toolkit/events/enums.h>

#include <hydra/util/callback.h>
#include <hydra/backend/layer_window.h>

#include <hydra/util/state_machine_impl.h>
#include <hydra/util/trie.h>
#include <window_manager.h>

using namespace miral;

namespace hydra::server {
  void ShellLauncher::enable_extensions(miral::WaylandExtensions& extensions) {
    for(auto ext: std::array {
        miral::WaylandExtensions::zwlr_layer_shell_v1,
        miral::WaylandExtensions::zwlr_foreign_toplevel_manager_v1,
      }) {
      extensions.conditionally_enable(ext, [this](miral::WaylandExtensions::EnableInfo const& info) {
        if(auto session = weak_session.lock()) {
          return session == info.app();
        }

        return false;
      });
    }
  }

  bool ShellLauncher::matches(miral::ApplicationInfo const& app) {
    if(auto session = weak_session.lock()) {
      return session == app.application();
    }

    return false;
  }

  void ShellLauncher::set_window_manager(WindowManager* wm) {
    this->wm = wm;
  }

  void ShellLauncher::stop() { shell.stop(); }

  void ShellLauncher::operator()(struct wl_display* display) {
    pthread_setname_np(pthread_self(), "HydraShell");

    // wait for the session to be set
    {
      std::unique_lock lock{session_lock};
      startup_cv.wait(lock, [this]{ return !weak_session.expired(); });
    }

    using namespace hydra::shell;

    SDLContext ctx(display);
    LayerWindow window(ctx, Window::Properties::FromConfig());

    shell.run(window, *state_machine);
  }

  void ShellLauncher::operator()(std::weak_ptr<mir::scene::Session> const session) {
    std::lock_guard g{session_lock};
    weak_session = session;
    startup_cv.notify_all();
  }

  enum States {
    IDLE,
    COMMAND,
    LAUNCH,
    WINDOW_FIND,
  };

  auto ShellLauncher::idle() {
    shell.show_status(title);
    return [](auto) -> std::size_t {
      throw std::runtime_error("Unreachable");
    };
  }

  namespace Commands {
    enum Commands: Option::value_t {
      NONE,
      QUIT,
      LAUNCH,
      WINDOW_FIND,
      WINDOW_NEXT,
      WINDOW_PREV,
      WINDOW_CLOSE,
    };

    auto opt(std::string_view name, Option::value_t value = NONE){ return Option{value, std::string{name}}; };
    static const hydra::util::Trie<Key, Option> tree {
      std::tuple{Key::Keycode(SDLK_Q), opt("Quit/logout session"),
        std::tuple{Key::Keycode(SDLK_Q), opt("Quit", QUIT)}},
      std::tuple{Key::Keycode(SDLK_SPACE), opt("Launch application", LAUNCH)},
      std::tuple{Key::Keycode(SDLK_W), opt("Windows"),
        std::tuple{Key::Keycode(SDLK_W), opt("Find window", WINDOW_FIND)},
        std::tuple{Key::Keycode(SDLK_N), opt("Next window", WINDOW_NEXT)},
        std::tuple{Key::Keycode(SDLK_P), opt("Prev window", WINDOW_PREV)},
        std::tuple{Key::Keycode(SDLK_D), opt("Close window", WINDOW_CLOSE)}}
    };

    auto show_node(auto* shell, auto const& node) {
      shell->show(util::collect_as<hydra::Table>(node.items() | std::views::transform([](auto const& p) {
        return std::pair {
          p.first,
          Option {Option::value_t(p.first.get()), p.second.name}
        };
      })));
    };
  }

  auto ShellLauncher::command() {
    Commands::show_node(&shell, Commands::tree.croot());

    std::string status = fmt::format("{} ", to_string(Config::Get().LEADER));
    shell.show_status(status);

    return [this,cur=Commands::tree.croot(),status](auto res) mutable -> std::size_t {
      auto key = Key::Raw(res < 0 ? -res - 1 : res);
      status = fmt::format("{}{} ", status, to_string(key));
      shell.show_status(status);

      if(res >= 0) {
        if(auto next = cur.try_get(key)) {
          cur = *next;
          switch(cur->value) {
            case Commands::QUIT:
              this->runner->stop();
              return States::IDLE;
            case Commands::LAUNCH:
              return States::LAUNCH;
            case Commands::WINDOW_FIND:
              return States::WINDOW_FIND;
            case Commands::WINDOW_NEXT:
              if(wm) wm->locked_advance_window(1);
              return States::IDLE;
            case Commands::WINDOW_PREV:
              if(wm) wm->locked_advance_window(-1);
              return States::IDLE;
            case Commands::WINDOW_CLOSE:
              if(wm) wm->locked_close_active();
              return States::IDLE;
            case Commands::NONE:
              Commands::show_node(&shell, cur);
              return -1;
            default:
              // unreachable
              break;
          }
        }
      } else {
        shell.show_error(fmt::format("{} undefined", status));
      }

      return States::IDLE;
    };
  }

  auto ShellLauncher::launch() {
    std::vector<Glib::RefPtr<Gio::AppInfo>> apps = Gio::AppInfo::get_all();
    std::ranges::stable_sort(apps, [](auto&& a, auto&& b) {
      return a->get_display_name() < b->get_display_name();
    });

    hydra::Search prompt;
    prompt.reserve(apps.size());

    Option::value_t idx = 0;
    for(auto app: apps) {
      prompt.emplace_back(idx++, app->get_display_name());
    }

    this->shell.show(std::move(prompt));

    return [this,apps=std::move(apps)](auto res) -> std::size_t {
      if(res >= 0 && res < apps.size()) {
        auto selected = apps[res];

        auto cmds = ExternalClientLauncher::split_command(selected->get_commandline());
        this->launcher->launch(cmds);
      }

      return States::IDLE;
    };
  }

  auto ShellLauncher::window_find() {
    std::vector<std::weak_ptr<WindowManager::Metadata>> windows;
    hydra::Search prompt;

    if(wm) wm->locked_list_windows([&](std::weak_ptr<WindowManager::Metadata> weak_window) {
      if(auto window = weak_window.lock()) {
        windows.push_back(weak_window);
        prompt.push_back(hydra::Option{(Option::value_t)prompt.size(), window->get_display_name()});
      }
    });

    shell.show(std::move(prompt));

    return [this,windows=std::move(windows)](auto res) -> std::size_t {
      if(res >= 0 && res < windows.size() && wm) {
        if(auto win = windows[res].lock(); win && wm) {
          wm->locked_select_window(win);
        }
      }

      return States::IDLE;
    };
  }

  ShellLauncher::ShellLauncher(MirRunner* runner, ExternalClientLauncher* launcher)
    : shell(true /* external input */),
      runner(runner),
      launcher(launcher),
      state_machine(StateMachine::Create<
                    ShellLauncher,
                    hydra::util::State { States::IDLE, &ShellLauncher::idle },
                    hydra::util::State { States::COMMAND, &ShellLauncher::command },
                    hydra::util::State { States::LAUNCH, &ShellLauncher::launch },
                    hydra::util::State { States::WINDOW_FIND, &ShellLauncher::window_find }
                    >(this))
  {
    Gio::init();
    runner->add_stop_callback([this](){ this->stop(); });
  }

  bool ShellLauncher::show_commands() {
    bool ret = false;
    state_machine->lock([&ret](std::size_t state) -> std::size_t {
      if(state == States::IDLE) {
        ret = true;
        return States::COMMAND;
      }

      return -1;
    });

    return ret;
  }

  bool ShellLauncher::handle_keyboard_event(MirKeyboardEvent const* event) {
    using namespace miral::toolkit;

    auto sc = mir_keyboard_event_scan_code(event);
    auto action = mir_keyboard_event_action(event);

    if(shell.wants_input() && action == mir_keyboard_action_down) {
      auto key = hydra::Key::Raw(sc);
      auto timestamp_ns = mir_input_event_get_event_time(mir_keyboard_event_input_event(event));
      shell.handle_key(key, timestamp_ns);
    } else if(sc == Config::Get().LEADER.get() && action == mir_keyboard_action_down) {
      show_commands();
    }

    return false;
  }

  void ShellLauncher::set_title(std::string_view s) {
    this->title = s;
    state_machine->lock([](std::size_t state) -> std::size_t {
      if(state == States::IDLE) {
        return States::IDLE;
      }

      return -1;
    });
  }
}
