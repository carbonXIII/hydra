#include <hydra/backend/layer_window.h>

#include <wayland-client-protocol.h>
#include <wayland-util.h>

#include <fmt/core.h>

#include <memory>

#include <hydra/backend/layer_shell_enum.h>
#include <backend/wayland.h>
#include <backend/layer_shell_wrapper.h>

namespace hydra::shell {
  struct LayerWindow::Self {
    Self(wl_display* display)
      : layer_shell(display)
    {}

    LayerShell layer_shell;

    std::weak_ptr<LayerSurface> layer_surface;
  };

  auto&& use_custom_role(Window::Properties&& props) {
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_WAYLAND_SURFACE_ROLE_CUSTOM_BOOLEAN, true);
    return std::forward<decltype(props)>(props);
  }

  LayerWindow::LayerWindow(const SDLContext& context, Window::Properties&& props)
    : Window(context, use_custom_role(std::forward<decltype(props)>(props))) {
    auto window_props = SDL_GetWindowProperties(get());

    auto* surface = static_cast<wl_surface*>(SDL_GetPointerProperty(window_props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr));

    if(!surface) {
      throw std::runtime_error("Could not retrieve wl_surface pointer from SDL_Window properties");
    }

    {
      auto* display = static_cast<wl_display*>(SDL_GetPointerProperty(window_props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr));

      if(!display) {
        throw std::runtime_error("Could not retrieve wl_display pointer from SDL_Window properties");
      }

      self = std::make_unique<Self>(display);
    }

    auto layer = shell::Layer(SDL_GetNumberProperty(props, LAYER_SHELL_PROP_LAYER, ZWLR_LAYER_SHELL_V1_LAYER_TOP));
    self->layer_surface = self->layer_shell.get_layer_surface(surface, layer, "");

    if(auto surface = self->layer_surface.lock()) {
      surface->on_configure([this](auto width, auto height) {
        SDL_SetWindowSize(get(), width, height);
      });

      auto width = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 0);
      auto height = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 0);
      auto anchors = SDL_GetNumberProperty(props, LAYER_SHELL_PROP_ANCHORS, 0);
      auto exclusive = SDL_GetNumberProperty(props, LAYER_SHELL_PROP_EXCLUSIVE_ZONE, 0);
      auto interactive = SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, true);

      surface->set_size(width, height);
      surface->set_anchors(anchors);
      surface->set_exclusive_zone(exclusive);
      surface->set_keyboard_interactivity(interactive ? shell::KEYBOARD_INTERACTIVITY_ON_DEMAND : shell::KEYBOARD_INTERACTIVITY_NONE);
    } else {
      throw std::runtime_error("Failed to get layer_surface for wl_surface");
    }

    // required empty commit
    wl_surface_commit(surface);
  }

  LayerWindow::~LayerWindow() {}
}
