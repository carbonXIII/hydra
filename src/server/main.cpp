#include <miral/runner.h>
#include <miral/x11_support.h>
#include <miral/internal_client.h>
#include <miral/set_window_management_policy.h>
#include <miral/minimal_window_manager.h>

#include <shell_launcher.h>

using namespace miral;

using namespace hydra::server;

int main(int argc, char const* argv[]) {
  MirRunner runner{argc, argv};

  ShellLauncher shell(&runner);

  WaylandExtensions extensions;
  shell.enable_extensions(extensions);

  return runner.run_with({
      X11Support{},
      extensions,
      StartupInternalClient{shell.internal_client()},
      set_window_management_policy<MinimalWindowManager>(),
    });
}
