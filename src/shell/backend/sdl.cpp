#include "SDL3/SDL_properties.h"
#include <hydra/backend/sdl.h>

#include <wayland-client-protocol.h>
#include <fmt/format.h>

#include <utility>

#ifndef NO_WAYLAND_EXTENSIONS
#include <hydra/backend/layer_shell_enum.h>
#endif

#include <hydra/config.h>

namespace hydra::shell {
  using Properties = Window::Properties;

  static void error_check(auto value) {
    if(!value) {
      throw std::runtime_error(fmt::format("SDL: {}", SDL_GetError()));
    }
  }

  SDLContext::SDLContext(wl_display* display) {
    if(display) {
      // We know we have a display, so avoid failing Init() if the env is not set
      SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "WAYLAND_DISPLAY", "", false);
      SDL_SetPointerProperty(SDL_GetGlobalProperties(), SDL_PROP_GLOBAL_VIDEO_WAYLAND_WL_DISPLAY_POINTER, display);
    }

    error_check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

    // GL(SL) version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  }

  SDLContext::~SDLContext() {
    SDL_Quit();
  }

  Window::Window(SDLContext const&, Properties&& props) {
    window.reset(SDL_CreateWindowWithProperties(props));
  }

  SDL_Window* Window::get() {
    return window.get();
  }

  SDL_GLContext Window::gl_context() {
    if(!_gl_context) {
      _gl_context.reset(SDL_GL_CreateContext(get()));
    }

    return _gl_context.get();
  }

  Window::operator SDL_Window*() {
    return get();
  }

  void Window::set_focusable(bool val) {
    SDL_SetWindowFocusable(get(), val);
  }

  void Window::raise() {
    SDL_RaiseWindow(get());
  }

  bool Window::has_focus() {
    auto flags = SDL_GetWindowFlags(get());

    if((flags & SDL_WINDOW_MINIMIZED)) {
      return false;
    }

    return (flags & SDL_WINDOW_INPUT_FOCUS);
  }

  Properties::Properties(): props(SDL_CreateProperties()) {}

  Properties::Properties(Properties&& o): props(std::exchange(o.props, {})) {}

  Properties::~Properties() {
    if(props) {
      SDL_DestroyProperties(props);
    }
  }

  Properties::operator SDL_PropertiesID() {
    return props;
  }

  Window::Properties Window::Properties::FromConfig() {
    Window::Properties props;

    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);

    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, Config::Get().WINDOW_HEIGHT);

#ifndef NO_WAYLAND_EXTENSIONS
    SDL_SetNumberProperty(props, LAYER_SHELL_PROP_LAYER, LAYER_OVERLAY);
    SDL_SetNumberProperty(props, LAYER_SHELL_PROP_ANCHORS, ANCHOR_BOTTOM | ANCHOR_LEFT | ANCHOR_RIGHT);
    SDL_SetNumberProperty(props, LAYER_SHELL_PROP_EXCLUSIVE_ZONE, Config::Get().BAR_HEIGHT);
    SDL_SetBooleanProperty(props, LAYER_SHELL_PROP_EXCLUSIVE, true);
#else
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, Config::Get().WINDOW_WIDTH);
#endif

    return props;
  }
}
