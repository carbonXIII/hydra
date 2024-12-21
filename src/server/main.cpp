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
#include <config_builder.h>

using namespace miral;
using namespace miral::toolkit;

using namespace hydra::server;

int main(int argc, char const* argv[]) {
  MirRunner runner{argc, argv};
  ExternalClientLauncher launcher;

  ShellLauncher shell(&runner, &launcher);

  WaylandExtensions extensions;
  shell.enable_extensions(extensions);

  ConfigBuilder config {
    ConfigOption {
      "leader", "Leader key to use to open the command table",
      std::in_place_type<mir::optional_value<std::string> const&>,
      [](hydra::Config& config, auto const& leader) {
        if(leader.is_set()) {
          config.LEADER = hydra::Key::String(leader.value().c_str());
        }
      },
    },
    ConfigOption {
      "font", "Name of the font to use for the shell",
      std::in_place_type<mir::optional_value<std::string> const&>,
      [optional_font=std::string{}](hydra::Config& config, auto const& font) mutable {
        if(font.is_set()) {
          optional_font = font.value();
          config.FONT = optional_font;
        }
      },
    }
  };

  return runner.run_with({
      X11Support{},
      extensions,
      StartupInternalClient{shell.internal_client()},
      set_window_management_policy<WindowManager>(shell),
      launcher,
      [&config](mir::Server& server) { config(server); }
    });
}
