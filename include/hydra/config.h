#pragma once

#include <SDL3/SDL_scancode.h>

#include <chrono>

#include <hydra/types.h>

namespace hydra {
  struct Config {
    /// Time to display error messages
    std::chrono::milliseconds ERROR_TIMEOUT { 1500 };

    unsigned BAR_HEIGHT = 50;
    unsigned TRIE_ROW_COUNT = 5;

    unsigned WINDOW_HEIGHT = BAR_HEIGHT * (TRIE_ROW_COUNT + 1);
    unsigned WINDOW_WIDTH = 640; // Ignored unless NO_WAYLAND_EXTENSIONS defined

    hydra::Key LEADER = hydra::Key::Scancode(SDL_SCANCODE_ESCAPE);

    static Config const& Get();
  };

  static constexpr Config base_config;
}
