#pragma once

#include <wlr-layer-shell-unstable-v1.hpp>

#include <memory>

#include <backend/wayland.h>
#include <hydra/backend/layer_shell_enum.h>

namespace hydra::shell {
  struct LayerSurface {
    using Base = CCZwlrLayerSurfaceV1;
    LayerSurface(wl_proxy* proxy);

    void set_anchors(uint32_t anchors);
    void set_size(uint32_t width, uint32_t height);
    void set_keyboard_interactivity(shell::KeyboardInteractivity val);
    void set_exclusive_zone(int32_t val);

    void on_configure(auto&& func) { on_configure_func = std::forward<decltype(func)>(func); }

  private:
    void configure(uint32_t serial, uint32_t width, uint32_t height) {
      if(on_configure_func) {
        on_configure_func(width, height);
      }
      self.sendAckConfigure(serial);
    }

    std::function<void(uint32_t, uint32_t)> on_configure_func;
    Base self;
  };

  struct LayerShell {
    LayerShell(wl_display* display);
    std::weak_ptr<LayerSurface> get_layer_surface(wl_surface* surface, shell::Layer layer, const std::string& nspace);

  private:
    CCZwlrLayerShellV1 self;
    std::vector<std::shared_ptr<LayerSurface>> surfaces;
  };
}
