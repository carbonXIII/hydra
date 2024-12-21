#include <mir_toolkit/events/enums.h>
#include <miral/runner.h>
#include <miral/x11_support.h>
#include <miral/internal_client.h>
#include <miral/set_window_management_policy.h>
#include <miral/append_event_filter.h>
#include <miral/minimal_window_manager.h>

#include <miral/toolkit_event.h>

#include <hydra/config.h>
#include <shell_launcher.h>

using namespace miral;
using namespace miral::toolkit;

using namespace hydra::server;

int main(int argc, char const* argv[]) {
  MirRunner runner{argc, argv};

  ShellLauncher shell(&runner);

  WaylandExtensions extensions;
  shell.enable_extensions(extensions);

  auto handle_keyboard_events = [&shell](MirEvent const* event) -> bool {
    if(mir_event_get_type(event) != mir_event_type_input) {
      return false;
    }

    auto* input_event = mir_event_get_input_event(event);
    if(mir_input_event_get_type(input_event) != mir_input_event_type_key) {
      return false;
    }

    auto* kev = mir_input_event_get_keyboard_event(input_event);
    if(mir_keyboard_event_action(kev) != mir_keyboard_action_down) {
      return false;
    }

    auto sc = mir_keyboard_event_scan_code(kev);
    if(sc == hydra::Config::Get().LEADER.get()) {
      return shell.show_commands();
    }

    return false;
  };

  return runner.run_with({
      X11Support{},
      extensions,
      StartupInternalClient{shell.internal_client()},
      set_window_management_policy<MinimalWindowManager>(),
      AppendEventFilter{handle_keyboard_events}
    });
}
