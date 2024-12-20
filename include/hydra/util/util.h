#pragma once

#include <string>
#include <string_view>
#include <source_location>
#include <vector>
#include <ranges>
#include <algorithm>
#include <functional>

#include <sstream>
#include <fmt/core.h>

namespace hydra::util {
  static std::string label(std::string_view sub,
                    const std::source_location& sloc = std::source_location::current()) {
    return fmt::format("##{}.{}", sloc.function_name(), sub);
  }
}
