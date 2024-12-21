#pragma once

#include <chrono>

namespace hydra {
  struct Config {
    /// Time to display error messages
    std::chrono::milliseconds ERROR_TIMEOUT { 1500 };

    unsigned BAR_HEIGHT = 50;
    unsigned TRIE_ROW_COUNT = 5;

    unsigned WINDOW_HEIGHT = BAR_HEIGHT * (TRIE_ROW_COUNT + 1);
    unsigned WINDOW_WIDTH = 640; // Ignored unless NO_WAYLAND_EXTENSIONS defined

    static Config const& Get();
  };

  static constexpr Config base_config;
}
