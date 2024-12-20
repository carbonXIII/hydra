#pragma once

#include <wayland-client-protocol.h>

#include <fmt/format.h>

#include <functional>
#include <string_view>
#include <stdexcept>
#include <memory>

namespace hydra::shell {
  template <typename Base, typename Wrapper, typename... Args>
  static constexpr void wrap(Base& base, void (Base::*setter)(std::function<void(Base*, Args...)>), void (Wrapper::*func)(Args...)) {
    std::invoke(setter, &base, [func](Base* base, Args&&... args) {
      std::invoke(func, static_cast<Wrapper*>(base->data()), std::forward<decltype(args)>(args)...);
    });
  }

  struct RegistryListener {
    RegistryListener(struct wl_display* display, wl_interface const* target, int version)
      : target(target), version(version)
    {
      auto* registry = wl_display_get_registry(display);
      wl_registry_add_listener(registry, &global_listener, this);
      wl_display_roundtrip(display);
      wl_registry_destroy(registry);
    }
    void global_create(wl_registry* registry, uint32_t id, const char* iface, uint32_t) {
      if(std::string_view{iface} == target->name) {
        value = wl_registry_bind(registry, id, target, version);
      }
    }
    static constexpr wl_registry_listener global_listener = {
      [](void* self, auto... args) { static_cast<RegistryListener*>(self)->global_create(args...); },
      [](auto... args) {}
    };

    template <typename T>
    T* get_or_die() {
      if(!value) {
        throw std::runtime_error(fmt::format("global {} not present", target->name));
      }

      return static_cast<T*>(value);
    }

  private:
    wl_interface const* target;
    int version;
    void* value;
  };

  namespace detail {
    struct CompositorDeleter {
      void operator()(wl_compositor* compositor) {
        wl_compositor_destroy(compositor);
      }
    };
  }

  using CompositorPtr = std::unique_ptr<wl_compositor, detail::CompositorDeleter>;

  static CompositorPtr get_compositor(struct wl_display* display) {
    return CompositorPtr(RegistryListener(display, &wl_compositor_interface, 3).get_or_die<wl_compositor>());
  }
}
