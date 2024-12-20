#pragma once

namespace hydra {
  struct Config {
    unsigned BAR_HEIGHT = 50;
    unsigned BAR_WIDTH = 640; // Ignored unless NO_WAYLAND_EXTENSIONS defined
    unsigned TRIE_ROW_COUNT = 5;

    static Config const& Get();
  };

  static constexpr Config base_config;
}
