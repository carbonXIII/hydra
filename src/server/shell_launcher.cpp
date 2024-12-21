#include <shell_launcher.h>

#include <pthread.h>

#include <hydra/util/callback.h>
#include <hydra/backend/layer_window.h>

#include <hydra/util/state_machine_impl.h>

using namespace miral;

namespace hydra::server {
  void ShellLauncher::enable_extensions(miral::WaylandExtensions& extensions) {
    for(auto ext: std::array {
        miral::WaylandExtensions::zwlr_layer_shell_v1,
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

  enum State {
    IDLE,
    COMMAND,
  };

  auto ShellLauncher::idle() {
    return [](auto) -> std::size_t {
      throw std::runtime_error("Unreachable");
    };
  }

  auto ShellLauncher::command() {
    enum Commands: Option::value_t {
      QUIT,
    };

    shell.show(hydra::Table{
        std::pair{hydra::Key::Keycode(SDLK_Q), hydra::Option{std::size_t(Commands::QUIT), "Quit"}},
      });

    return [this](auto res) -> std::size_t {
      if(res < 0) {
        auto key = hydra::Key::Raw(-res - 1);
        shell.show_error(fmt::format("{} Undefined", to_string(key)));
      } else {
        switch(Commands(res)) {
          case Commands::QUIT:
            runner->stop();
            break;
          default:
            break;
        }
      }

      return State::IDLE;
    };
  }

  ShellLauncher::ShellLauncher(MirRunner* runner)
    : runner(runner),
      state_machine(StateMachine::Create<
                    ShellLauncher,
                    hydra::util::State { State::IDLE, &ShellLauncher::idle },
                    hydra::util::State { State::COMMAND, &ShellLauncher::command }
                    >(this))
  {
    runner->add_stop_callback([this](){ this->stop(); });
  }

  bool ShellLauncher::show_commands() {
    bool ret = false;
    state_machine->lock([&ret](std::size_t state) -> std::size_t {
      if(state == State::IDLE) {
        ret = true;
        return State::COMMAND;
      }

      return -1;
    });

    return ret;
  }
}
