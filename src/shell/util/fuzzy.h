#pragma once

#include <rapidfuzz/fuzz.hpp>

#include <string_view>

#include <hydra/util/util.h>

namespace hydra::util {
  struct FuzzyFilter {
    FuzzyFilter(std::string_view s): valid(!s.empty()), cache(util::tolower(s)) {}

    bool operator()(std::string_view s) {
      if(!valid) return true;
      auto v = cache.similarity(util::tolower(s), 100.F);
      return v > 0.;
    }

  private:
    bool valid;
    rapidfuzz::fuzz::CachedPartialTokenSortRatio<char> cache;
  };
}
