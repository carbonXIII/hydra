#include <window_manager.h>

#include <miral/toolkit_event.h>

using namespace miral;

namespace hydra::server {
  WindowManager::WindowManager(WindowManagerTools const& tools, ShellLauncher& shell)
    : Base(tools),
      shell_launcher(&shell)
  {}

  bool WindowManager::handle_keyboard_event(MirKeyboardEvent const* event) {
    using namespace miral::toolkit;

    if(shell_launcher->handle_keyboard_event(event)) {
      return true;
    }

    return false;
  }
}
