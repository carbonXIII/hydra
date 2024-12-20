#pragma once

#include <string_view>
#include <chrono>

#include <hydra/types.h>
#include <hydra/util/util.h>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <fmt/format.h>
#include <fmt/chrono.h>

namespace hydra::shell {
  struct StatusLine {
    void show(std::string_view s) {
      status = s;
    }

    void show(std::string_view s, const clock_t::duration& duration) {
      message = s;
      expiration = clock_t::now() + duration;
    }

    void draw() {
      if(ImGui::Begin(util::label("window").c_str(), nullptr,
                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize)) {
        if(ImGui::BeginTable(util::label("table").c_str(), 2)) {
          ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
          ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
          ImGui::TableNextRow();
          if(ImGui::TableSetColumnIndex(0)) {
            if(!message.empty() && clock_t::now() > expiration) message.clear();
            ImGui::TextUnformatted(message.empty() ? status.c_str() : message.c_str());
          }

          if(ImGui::TableSetColumnIndex(1)) {
            auto date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::string date_s = fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(date));
            ImGui::TextUnformatted(date_s.c_str());
          }

          ImGui::EndTable();
        }

        ImGui::End();
      }
    }

  private:
    std::string status;

    std::string message;
    clock_t::time_point expiration;
  };
}
