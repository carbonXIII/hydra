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
      : layer_shell(display),
        compositor(GetCompositor(display))
    {}

    LayerShell layer_shell;
    CompositorPtr compositor;

    std::weak_ptr<LayerSurface> layer_surface;

    bool exclusive;
    bool interactive;
  };

  auto&& use_custom_role(Window::Properties&& props) {
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_WAYLAND_SURFACE_ROLE_CUSTOM_BOOLEAN, true);
    return std::forward<decltype(props)>(props);
  }

  static constexpr auto GetKeyboardInteract(bool interactive, bool exclusive) {
    if(!interactive) return shell::KEYBOARD_INTERACTIVITY_NONE;
    if(!exclusive) return shell::KEYBOARD_INTERACTIVITY_ON_DEMAND;
    return shell::KEYBOARD_INTERACTIVITY_EXCLUSIVE;
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
      auto exclusive_zone = SDL_GetNumberProperty(props, LAYER_SHELL_PROP_EXCLUSIVE_ZONE, 0);
      self->interactive = SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, true);
      self->exclusive = SDL_GetBooleanProperty(props, LAYER_SHELL_PROP_EXCLUSIVE, false);

      surface->set_size(width, height);
      surface->set_anchors(anchors);
      surface->set_exclusive_zone(exclusive_zone);
      surface->set_keyboard_interactivity(GetKeyboardInteract(self->interactive, self->exclusive));
    } else {
      throw std::runtime_error("Failed to get layer_surface for wl_surface");
    }

    // required empty commit
    wl_surface_commit(surface);
  }

  void LayerWindow::set_exclusive(bool val) {
    self->exclusive = val;

    if(auto surface = self->layer_surface.lock()) {
      surface->set_keyboard_interactivity(GetKeyboardInteract(self->interactive, self->exclusive));
    }

    if(auto* w_surface = static_cast<wl_surface*>(SDL_GetPointerProperty(SDL_GetWindowProperties(get()),
                                                                         SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr))) {
      // empty commit for changes to take effect
      wl_surface_attach(w_surface, nullptr, 0, 0);
      wl_surface_commit(w_surface);
    }
  }

  void LayerWindow::set_focusable(bool focus) {
    self->interactive = focus;
    if(auto surface = self->layer_surface.lock()) {
      surface->set_keyboard_interactivity(GetKeyboardInteract(self->interactive, self->exclusive));
    }

    if(auto* w_surface = static_cast<wl_surface*>(SDL_GetPointerProperty(SDL_GetWindowProperties(get()),
                                                                         SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr))) {
      if(!focus) {
        auto* region = static_cast<wl_region*>(wl_compositor_create_region(self->compositor.get()));
        wl_surface_set_input_region(w_surface, region);
        wl_region_destroy(region);
      } else {
        wl_surface_set_input_region(w_surface, nullptr);
      }

      // empty commit for changes to take effect
      wl_surface_attach(w_surface, nullptr, 0, 0);
      wl_surface_commit(w_surface);
    }

    SDL_SetWindowFocusable(get(), focus);
  }

  LayerWindow::~LayerWindow() {}
}
