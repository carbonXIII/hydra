#include <mir_toolkit/events/enums.h>
#include <miral/runner.h>
#include <miral/x11_support.h>
#include <miral/internal_client.h>
#include <miral/external_client.h>
#include <miral/set_window_management_policy.h>
#include <miral/append_event_filter.h>

#include <miral/toolkit_event.h>

#include <hydra/config.h>
#include <shell_launcher.h>
#include <window_manager.h>

using namespace miral;
using namespace miral::toolkit;

using namespace hydra::server;

int main(int argc, char const* argv[]) {
  MirRunner runner{argc, argv};
  ExternalClientLauncher launcher;

  ShellLauncher shell(&runner, &launcher);

  WaylandExtensions extensions;
  shell.enable_extensions(extensions);

  return runner.run_with({
      X11Support{},
      extensions,
      StartupInternalClient{shell.internal_client()},
      set_window_management_policy<WindowManager>(shell),
      launcher
    });
}
