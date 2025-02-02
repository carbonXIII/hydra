#pragma once

#include <string>
#include <optional>

namespace hydra {
  struct FontConfig {
    std::optional<std::string> match_font(std::string const& pattern_str);

    static FontConfig& get() {
      static FontConfig fc;
      return fc;
    }

  private:
    FontConfig();
    ~FontConfig();
  };
}
