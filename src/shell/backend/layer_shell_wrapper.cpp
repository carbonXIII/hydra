#include <backend/layer_shell_wrapper.h>

namespace hydra::shell {
  LayerSurface::LayerSurface(wl_proxy* proxy): self(proxy) {
    self.setData(this);
    wrap(self, &Base::setConfigure, &LayerSurface::configure);
  }

  void LayerSurface::set_anchors(uint32_t anchors) { self.sendSetAnchor(::zwlrLayerSurfaceV1Anchor(anchors)); }
  void LayerSurface::set_size(uint32_t width, uint32_t height) { self.sendSetSize(width, height); }
  void LayerSurface::set_keyboard_interactivity(shell::KeyboardInteractivity val) { self.sendSetKeyboardInteractivity(::zwlrLayerSurfaceV1KeyboardInteractivity(val)); }
  void LayerSurface::set_exclusive_zone(int32_t val) { self.sendSetExclusiveZone(val); }

  LayerShell::LayerShell(wl_display* display):
    self(RegistryListener(display, &zwlr_layer_shell_v1_interface, 3).get_or_die<wl_resource>())
  {}

  std::weak_ptr<LayerSurface> LayerShell::get_layer_surface(wl_surface* surface, shell::Layer layer, const std::string& nspace) {
    wl_proxy* proxy = self.sendGetLayerSurface
      (
       reinterpret_cast<wl_resource*>(surface),
       nullptr /* output */,
       ::zwlrLayerShellV1Layer(layer),
       nspace.c_str());

    auto layer_surface = std::make_shared<LayerSurface>(proxy);
    std::weak_ptr ret = layer_surface;
    surfaces.emplace_back(std::move(layer_surface));
    return ret;
  }
}
