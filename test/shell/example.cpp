#include <fmt/core.h>

#include <chrono>

#include <imgui/imgui.h>

#include <hydra/config.h>
#include <hydra/backend/sdl.h>
#ifndef NO_WAYLAND_EXTENSIONS
#include <hydra/backend/layer_window.h>
#endif

#include <shell/backend/imgui.h>

using namespace hydra::shell;

int main() {
  SDLContext context;

#ifndef NO_WAYLAND_EXTENSIONS
  using Window = hydra::shell::LayerWindow;
#else
  using Window = hydra::Window;
#endif

  Window window(context, Window::Properties::FromConfig());

  FrameContext frame_context(&window);

  {
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
  }

  bool done = false;
  while(!done) {
    frame_context.handle_events([&](SDL_Event const& e) {
      if(e.type == SDL_EVENT_QUIT) {
        done = true;
      } else if(e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_Q) {
        done = true;
      }
    });

    {
      auto frame = frame_context.start_frame();

      auto& io = ImGui::GetIO();
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
      if(ImGui::Begin("Hydra", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Hello imgui");
        ImGui::End();
      }

      frame.should_show();
    }
  }
}
