#pragma once

#include <concepts>
#include <optional>

#include <imgui/imgui.h>

#include <hydra/types.h>
#include <shell/table.h>
#include <shell/search.h>
#include <shell/statusline.h>

namespace hydra::shell {
  template <typename T>
  concept IsDrawable = requires(T& t) {
    { t.should_draw() } -> std::convertible_to<bool>;
    { t.draw() };
  };

  static_assert(IsDrawable<StatusLine>);

  template <typename T>
  concept IsPrompt = IsDrawable<T> && requires(T& t) {
    { t.handle_key(std::declval<Key>()) };
    { t.try_result() } -> std::convertible_to<std::optional<Option::value_t>>;
  };

  static_assert(IsPrompt<TablePrompt>);
  static_assert(IsPrompt<SearchPrompt>);
  using Prompt = std::variant<TablePrompt, SearchPrompt>;

  static void draw(Prompt& prompt) {
    std::visit([](auto& p){ p.draw(); }, prompt);
  }

  static bool should_draw(Prompt const& prompt) {
    return std::visit([](auto& p){ return p.should_draw(); }, prompt);
  }

  static void handle_key(Prompt& prompt, const Key& key) {
    std::visit([&](auto& p){ p.handle_key(key); }, prompt);
  }

  static std::optional<Option::value_t> try_result(Prompt& prompt) {
    return std::visit([](auto& p){ return p.try_result(); }, prompt);
  }
}
