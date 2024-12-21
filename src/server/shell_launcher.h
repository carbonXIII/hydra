#pragma once

#include <miral/runner.h>
#include <miral/wayland_extensions.h>

#include <hydra/shell.h>
#include <hydra/types.h>
#include <hydra/util/state_machine.h>

#include <condition_variable>
#include <mutex>

namespace hydra::server {
  struct ShellLauncher {
    ShellLauncher(miral::MirRunner* runner);
    ShellLauncher(ShellLauncher const&) = delete;
    ShellLauncher(ShellLauncher&&) = delete;

    auto internal_client() {
      return [this](auto&& args) {
        operator()(std::forward<decltype(args)>(args));
      };
    }

    void enable_extensions(miral::WaylandExtensions& extensions);

    void stop();

  private:
    void operator()(struct wl_display* display);
    void operator()(std::weak_ptr<mir::scene::Session> const session);

    hydra::shell::Shell shell;

    miral::MirRunner* runner;

    std::weak_ptr<mir::scene::Session> weak_session;
    std::mutex session_lock;
    std::condition_variable startup_cv;
  };
}
