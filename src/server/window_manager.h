#pragma once

#include <miral/minimal_window_manager.h>
#include <miral/toolkit_event.h>
#include <miral/window_manager_tools.h>

#include <fmt/format.h>

#include <shell_launcher.h>

namespace hydra::server {
  struct WindowManager: miral::MinimalWindowManager {
    using Base = miral::MinimalWindowManager;

    WindowManager(miral::WindowManagerTools const& tools, ShellLauncher& shell);

    virtual bool handle_keyboard_event(MirKeyboardEvent const* event) override;

  private:
    ShellLauncher* shell_launcher;
  };
}
