#include "search.h"

#include <util/fuzzy.h>

namespace hydra::shell {
  void SearchPrompt::handle_filter(std::string_view s) {
    util::FuzzyFilter filter{s};
    auto score = util::collect(options | std::views::transform([&](Option option) {
      return filter(option.name);
    }));

    filtered = util::collect(std::ranges::iota_view(0u, options.size())
                             | std::views::filter([&](auto idx){ return score[idx]; }));

    if(auto it = std::ranges::lower_bound(filtered, cursor_idx); it != filtered.end()) {
      cursor_idx = *it;
    } else if(filtered.size()) {
      cursor_idx = filtered.back();
    }
  }
}
