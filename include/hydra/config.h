#pragma once

#include <SDL3/SDL_scancode.h>

#include <chrono>

#include <hydra/types.h>

namespace hydra {
  struct Config {
    /// Target frame time (1 / refresh)
    std::chrono::milliseconds FRAME_TIMEOUT { 1000 / 60 };

    /// Time to wait before resetting if we can't gain focus
    std::chrono::milliseconds FOCUS_TIMEOUT { 500 };

    /// Time to display error messages
    std::chrono::milliseconds ERROR_TIMEOUT { 1500 };

    /// Number of frames to buffer inputs without a prompt consuming them
    uint64_t BUFFER_FRAMES = 5;

    std::string_view FONT = "Monospace";
    unsigned FONT_HEIGHT = 30;

    double ALPHA = 0.7;
    unsigned BAR_HEIGHT = FONT_HEIGHT + 20;
    unsigned TABLE_ROW_COUNT = 5;

    unsigned WINDOW_HEIGHT = BAR_HEIGHT * (TABLE_ROW_COUNT + 1);
    unsigned WINDOW_WIDTH = 640; // Ignored unless NO_WAYLAND_EXTENSIONS defined

    hydra::Key LEADER = hydra::Key::Scancode(SDL_SCANCODE_LGUI);

    template <typename Dur>
    constexpr auto buffer_timeout() const {
      return BUFFER_FRAMES * std::chrono::duration_cast<Dur>(FRAME_TIMEOUT).count();
    }

    struct [[nodiscard]] Handle {
      Handle();
      Handle(Handle const&) = delete;
      Handle(Handle&&) = delete;
      Handle& operator=(Handle&& o);
      ~Handle();

    protected:
      friend Config;

      Handle(bool active);

      bool active = false;
    };

    static Config const& Get();
    static Handle Override(Config const& config);
  };

  static constexpr Config base_config;
}
