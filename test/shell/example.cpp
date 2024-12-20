#include <fmt/core.h>

#include <hydra/backend/sdl.h>

#include <shell/backend/imgui.h>
#include <imgui/imgui.h>

using namespace hydra::shell;

auto get_window_props() {
  Window::Properties props;
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, true);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 640);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 480);
  return props;
}

int main() {
  SDLContext context;
  Window window(context, get_window_props());

  SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
  SDL_ShowWindow(window);

  FrameContext frame_context(&window);

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

      if(ImGui::Begin("Hydra")) {
        ImGui::Text("Hello imgui");
        ImGui::End();
      }

      frame.should_show();
    }
  }
}
