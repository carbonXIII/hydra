#pragma once

#include <fmt/core.h>
#include <imgui/imgui.h>

#include <chrono>
#include <vector>
#include <algorithm>

#include <hydra/config.h>
#include <hydra/types.h>
#include <hydra/util/util.h>

namespace hydra::shell {
  struct TablePrompt {
    TablePrompt(auto&& options, clock_t::time_point show_time = {})
    : options(std::forward<decltype(options)>(options)),
      show_time(show_time) {
      auto cmp = [](auto&& l, auto&& r) { return l.first < r.first; };
      std::ranges::sort(this->options, cmp);
    }

    bool should_draw() const {
      return clock_t::now() > show_time;
    }

    std::optional<Option::value_t> try_result() {
      return ret;
    }

    void handle_key(Key key) {
      auto cmp = [](auto&& p, auto&& k) { return p.first < k; };

      if(auto it = std::lower_bound(options.begin(), options.end(), key, cmp);
         it != options.end() && it->first == key) {
        ret = it->second.value;
      } else {
        ret = key.get();
      }
    }

    void draw() {
      if(ImGui::Begin(util::label("window").c_str(), nullptr,
                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize)) {
        const unsigned ncols = (options.size() + Config::Get().TRIE_ROW_COUNT - 1) / Config::Get().TRIE_ROW_COUNT;
        if(ImGui::BeginTable(util::label("table").c_str(), ncols * 3)) {
          for(unsigned i = 0; i < ncols; ++i) {
            ImGui::TableSetupColumn(util::label(fmt::format("key#{}", i)).c_str(), ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn(util::label(fmt::format("col#{}", i)).c_str(), ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn(util::label(fmt::format("cmd#{}", i)).c_str(), ImGuiTableColumnFlags_WidthFixed);
          }

          for(int i = 0; i < Config::Get().TRIE_ROW_COUNT; i++) {
            ImGui::TableNextRow();
            for(int j = 0; j < ncols; ++j) {
              if(int idx = j * Config::Get().TRIE_ROW_COUNT + i; idx < options.size()) {
                if(ImGui::TableSetColumnIndex(j * 3 + 0)) {
                  ImGui::TextUnformatted(to_string(options[idx].first).c_str());
                }

                if(ImGui::TableSetColumnIndex(j * 3 + 1)) {
                  ImGui::TextUnformatted(":");
                }

                if(ImGui::TableSetColumnIndex(j * 3 + 2)) {
                  ImGui::TextUnformatted(options[idx].second.name.c_str());
                }
              }
            }
          }

          ImGui::EndTable();
        }

        ImGui::End();
      }
    }

  private:
    std::vector<std::pair<Key, Option>> options;
    const clock_t::time_point show_time;
    std::optional<Option::value_t> ret;
  };
}
