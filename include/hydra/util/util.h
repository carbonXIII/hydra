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

  template <typename container_t>
  static auto collect_as(auto&& r) {
    // https://timur.audio/how-to-make-a-container-from-a-c20-range
    container_t v;

    if constexpr(std::ranges::sized_range<decltype(r)>) {
      v.reserve(std::ranges::size(r));
    }

    std::ranges::copy(r, std::back_inserter(v));
    return v;
  }

  static auto collect(auto&& r) {
    return collect_as<std::vector<std::ranges::range_value_t<decltype(r)>>>(r);
  }

  static std::string tolower(auto&& s) {
    std::string ret(std::forward<decltype(s)>(s));
    for(auto& ch: ret) ch = std::tolower(ch);
    return ret;
  }

  template <typename... T>
  struct overloaded: T... {
    using T::operator()...;
  };

  template <typename... T>
  overloaded(T...) -> overloaded<T...>;
}
