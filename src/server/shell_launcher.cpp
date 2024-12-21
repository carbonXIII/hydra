#include <shell_launcher.h>

#include <pthread.h>
#include <giomm/init.h>
#include <giomm/appinfo.h>

#include <hydra/util/callback.h>
#include <hydra/backend/layer_window.h>

#include <hydra/util/state_machine_impl.h>
#include <hydra/util/trie.h>

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
    LAUNCH
  };

  auto ShellLauncher::idle() {
    return [](auto) -> std::size_t {
      throw std::runtime_error("Unreachable");
    };
  }

  auto ShellLauncher::command() {
    enum Commands: Option::value_t {
      NONE,
      QUIT,
      LAUNCH,
    };

    static constexpr auto opt = [](std::string_view name, Option::value_t value = NONE){ return Option{value, std::string{name}}; };
    hydra::util::Trie<Key, Option> tree {
      std::tuple{Key::Keycode(SDLK_Q), opt("Quit/logout session"),
        std::tuple{Key::Keycode(SDLK_Q), opt("Quit", QUIT)}},
      std::tuple{Key::Keycode(SDLK_SPACE), opt("Launch application", LAUNCH)}
    };

    static constexpr auto show_node = [](auto* shell, auto const& node) {
      shell->show(util::collect_as<hydra::Table>(node.items() | std::views::transform([](auto const& p) {
        return std::pair {
          p.first,
          Option {Option::value_t(p.first.get()), p.second.name}
        };
      })));
    };
    show_node(&shell, tree.root());

    std::string status = fmt::format("{} ", to_string(Config::Get().LEADER));
    shell.show_status(status);

    using node = std::optional<decltype(tree)::node_t>;
    return [this,tree,cur=node{},status](auto res) mutable -> std::size_t {
      if(!cur) {
        cur = tree.root();
      }

      auto key = Key::Raw(res < 0 ? -res - 1 : res);
      status = fmt::format("{}{} ", status, to_string(key));
      shell.show_status(status);

      if(res >= 0) {
        if(auto next = cur->try_get(key)) {
          cur = *next;
          switch(Commands{(*cur)->value}) {
            case QUIT:
              this->runner->stop();
              return States::IDLE;
            case LAUNCH:
              return States::LAUNCH;
            case NONE:
              show_node(&shell, *cur);
              return -1;
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

  ShellLauncher::ShellLauncher(MirRunner* runner, ExternalClientLauncher* launcher)
    : runner(runner),
      launcher(launcher),
      state_machine(StateMachine::Create<
                    ShellLauncher,
                    hydra::util::State { States::IDLE, &ShellLauncher::idle },
                    hydra::util::State { States::COMMAND, &ShellLauncher::command },
                    hydra::util::State { States::LAUNCH, &ShellLauncher::launch }
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
}
