#include <shell_launcher.h>

#include <pthread.h>

#include <hydra/util/callback.h>
#include <hydra/backend/layer_window.h>

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

    auto cb = Shell::Callback::Create([](auto){ /* do nothing */ });
    shell.run(window, cb);
  }

  void ShellLauncher::operator()(std::weak_ptr<mir::scene::Session> const session) {
    std::lock_guard g{session_lock};
    weak_session = session;
    startup_cv.notify_all();
  }

  ShellLauncher::ShellLauncher(MirRunner* runner)
    : runner(runner) {
    runner->add_stop_callback([this](){ this->stop(); });
  }
}
