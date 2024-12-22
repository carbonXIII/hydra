#pragma once

#include <miral/application_info.h>
#include <miral/runner.h>
#include <miral/wayland_extensions.h>
#include <miral/external_client.h>
#include <miral/toolkit_event.h>

#include <hydra/shell.h>
#include <hydra/types.h>

#include <condition_variable>
#include <mutex>

namespace hydra::util { template <typename Input> struct StateMachine; }
namespace hydra::server {
  struct WindowManager;
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
    bool matches(miral::ApplicationInfo const& app);
    void set_window_manager(WindowManager* wm);

    void stop();
    bool show_commands();

    bool handle_keyboard_event(MirKeyboardEvent const* event);
    void set_title(std::string_view s);

  private:
    void operator()(struct wl_display* display);
    void operator()(std::weak_ptr<mir::scene::Session> const session);

    auto idle();
    auto command();
    auto launch();
    auto window_find();

    hydra::shell::Shell shell;

    miral::MirRunner* runner;
    miral::ExternalClientLauncher* launcher;
    WindowManager* wm = nullptr;

    std::weak_ptr<mir::scene::Session> weak_session;
    std::mutex session_lock;
    std::condition_variable startup_cv;

    using StateMachine = util::StateMachine<Option::value_t>;
    std::shared_ptr<StateMachine> state_machine;

    std::string title;
  };
}
