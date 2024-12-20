#include <fmt/core.h>

#include <chrono>

#include <imgui/imgui.h>

#include <hydra/config.h>
#include <hydra/backend/sdl.h>
#ifndef NO_WAYLAND_EXTENSIONS
#include <hydra/backend/layer_window.h>
#endif

#include <shell/backend/imgui.h>
#include <shell/statusline.h>

using namespace hydra::shell;

int main() {
  SDLContext context;

#ifndef NO_WAYLAND_EXTENSIONS
  using Window = hydra::shell::LayerWindow;
#else
  using Window = hydra::Window;
#endif

  StatusLine status;

  Window window(context, Window::Properties::FromConfig());

  FrameContext frame_context(&window);

  {
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
  }

  bool done = false;

  int test_counter = 0;
  while(!done) {
    frame_context.handle_events([&](SDL_Event const& e) {
      if(e.type == SDL_EVENT_QUIT) {
        done = true;
      } else if(e.type == SDL_EVENT_KEY_DOWN) {
        switch(e.key.key) {
          case SDLK_Q:
            done = true;
            break;
          case SDLK_H:
            status.show("hello status line", std::chrono::milliseconds(5000));
            break;
          case SDLK_RETURN:
            ++test_counter;
            status.show(fmt::format("Counter: {}", test_counter));
          default:
            break;
        }
      }
    });

    {
      auto frame = frame_context.start_frame();

      auto& io = ImGui::GetIO();
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
      status.draw();

      frame.should_show();
    }
  }
}
