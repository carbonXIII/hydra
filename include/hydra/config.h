#pragma once

namespace hydra {
  struct Config {
    unsigned BAR_HEIGHT = 50;
    unsigned TRIE_ROW_COUNT = 5;

    unsigned WINDOW_HEIGHT = BAR_HEIGHT * (TRIE_ROW_COUNT + 1);
    unsigned WINDOW_WIDTH = 640; // Ignored unless NO_WAYLAND_EXTENSIONS defined

    static Config const& Get();
  };

  static constexpr Config base_config;
}
