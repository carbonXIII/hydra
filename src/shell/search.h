#pragma once

#include <SDL3/SDL_keycode.h>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <vector>
#include <utility>

#include <hydra/types.h>
#include <hydra/util/util.h>

namespace hydra::shell {
  struct SearchPrompt {
    SearchPrompt(auto&&... options)
    : options(std::forward<decltype(options)>(options)...),
      filtered(util::collect(std::ranges::iota_view(0u, this->options.size())))
      {
      imgui_selection.UserData = this;
      imgui_selection.AdapterSetItemSelected = [](ImGuiSelectionExternalStorage* self, int idx, bool selected) {
        if(selected) {
          reinterpret_cast<SearchPrompt*>(self->UserData)->cursor_idx = idx;
        }
      };
    }

    bool should_draw() const {
      return true;
    }

    void handle_key(Key key) {
      auto keycode = key.keycode();
      if(keycode == SDLK_DOWN || keycode == SDLK_UP) {
        if(auto it = std::find(filtered.begin(), filtered.end(), cursor_idx); it != filtered.end()) {
          if(keycode == SDLK_DOWN && next(it) != filtered.end())
            std::advance(it, 1);
          if(keycode == SDLK_UP && it != filtered.begin())
            std::advance(it, -1);
          cursor_idx = *it;
          damage = true;
        }
      } else if(keycode == SDLK_RETURN && filtered.size() && cursor_idx < options.size()) {
        ret = options[cursor_idx].value;
      } else if(keycode == SDLK_ESCAPE && !ret) {
        ret = -1;
      }
    }

    std::optional<Option::value_t> try_result() {
      return ret;
    }

    void draw() {
      if(ImGui::Begin(util::label("window").c_str(), nullptr,
                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize)) {
        if(ImGui::InputText(util::label("filter").c_str(), &filter_str)) {
          handle_filter(filter_str);
        }

        ImGui::SetItemDefaultFocus();
        if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
          ImGui::SetKeyboardFocusHere(-1);

        int visible_lo = -1, visible_hi = -1;
        if(ImGui::BeginTable(util::label("select").c_str(), 1, ImGuiTableFlags_ScrollY)) {
          ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
          auto flags = ImGuiMultiSelectFlags_SingleSelect | ImGuiMultiSelectFlags_NoAutoSelect;
          auto ms_io = ImGui::BeginMultiSelect(flags);
          imgui_selection.ApplyRequests(ms_io);

          auto labels = filtered | std::views::transform([this](unsigned idx) {
            return std::pair<unsigned, std::string_view>{idx, options[idx].name};
          });

          bool scroll_pending = false;
          for(auto&& [idx, option]: labels) {
            ImGui::TableNextRow();
            if(ImGui::TableSetColumnIndex(0)) {
              ImGui::SetNextItemSelectionUserData(idx);

              bool is_selected = idx == this->cursor_idx;
              ImGui::Selectable(std::string(option).c_str(), is_selected);

              if(is_selected && std::exchange(damage, false)) {
                ImGui::SetScrollHereY(0.5f);
                scroll_pending = true;
              }

              if(ImGui::IsItemVisible()) {
                if(visible_lo < 0) visible_lo = idx;
                visible_hi = idx;
              }
            }
          }

          if(!scroll_pending && visible_lo >= 0) {
            this->cursor_idx = std::clamp((int)this->cursor_idx, visible_lo, visible_hi);
          }

          ms_io = ImGui::EndMultiSelect();
          imgui_selection.ApplyRequests(ms_io);

          ImGui::PopStyleColor();
          ImGui::EndTable();
        }

        ImGui::End();
      }
    }

  private:
    void handle_filter(std::string_view s);

    const std::vector<Option> options;

    std::string filter_str;
    std::vector<unsigned> filtered;
    unsigned cursor_idx = 0;

    std::optional<Option::value_t> ret;

    ImGuiSelectionExternalStorage imgui_selection;
    bool damage;
  };
}
