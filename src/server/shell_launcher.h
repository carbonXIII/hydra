#pragma once

#include <miral/runner.h>
#include <miral/wayland_extensions.h>
#include <miral/external_client.h>

#include <hydra/shell.h>
#include <hydra/types.h>

#include <condition_variable>
#include <mutex>

namespace hydra::util { template <typename Input> struct StateMachine; }
namespace hydra::server {
  struct ShellLauncher {
    ShellLauncher(miral::MirRunner* runner, miral::ExternalClientLauncher* launcher);
    ShellLauncher(ShellLauncher const&) = delete;
    ShellLauncher(ShellLauncher&&) = delete;

    auto internal_client() {
      return [this](auto&& args) {
        operator()(std::forward<decltype(args)>(args));
      };
    }

    void enable_extensions(miral::WaylandExtensions& extensions);

    void stop();
    bool show_commands();

  private:
    void operator()(struct wl_display* display);
    void operator()(std::weak_ptr<mir::scene::Session> const session);

    auto idle();
    auto command();
    auto launch();

    hydra::shell::Shell shell;

    miral::MirRunner* runner;
    miral::ExternalClientLauncher* launcher;

    std::weak_ptr<mir::scene::Session> weak_session;
    std::mutex session_lock;
    std::condition_variable startup_cv;

    using StateMachine = util::StateMachine<Option::value_t>;
    std::shared_ptr<StateMachine> state_machine;
  };
}
