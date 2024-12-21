#include <backend/imgui.h>

#include <imgui/imgui.h>

#include <string>

namespace hydra::shell {
  void ImGuiLoadConfig(ImGuiStyle& style) {
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = nullptr;
  }
}
