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
#include <shell/table.h>

using namespace hydra::shell;

enum Event: hydra::Option::value_t {
  QUIT,
  COUNTER,
  HELLO,
};

int main() {
  SDLContext context;

#ifndef NO_WAYLAND_EXTENSIONS
  using Window = hydra::shell::LayerWindow;
#else
  using Window = hydra::Window;
#endif

  StatusLine status;
  std::optional<TablePrompt> table;

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
        if(table) {
          table->handle_key(hydra::Key::Raw(e.key.raw));

          if(auto res = table->try_result()) {
            switch(Event(*res)) {
              case QUIT:
                done = true;
                break;
              case COUNTER:
                ++test_counter;
                status.show(fmt::format("Counter: {}", test_counter));
                break;
              case HELLO:
                status.show("hello status line", std::chrono::milliseconds(5000));
                break;
              default:
                break;
            }

            table.reset();
          }
        }

        if(!table) {
          switch(e.key.key) {
            case SDLK_SPACE:
              table.emplace(hydra::Table{
                  std::pair{hydra::Key::Scancode(SDL_SCANCODE_H), hydra::Option{HELLO, "Hello"}},
                  std::pair{hydra::Key::Scancode(SDL_SCANCODE_C), hydra::Option{COUNTER, "Counter++"}},
                  std::pair{hydra::Key::Scancode(SDL_SCANCODE_Q), hydra::Option{QUIT, "Quit"}},
                });
              break;
            default:
              break;
          }
        }
      }
    });

    {
      auto frame = frame_context.start_frame();

      auto& io = ImGui::GetIO();
      ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - hydra::Config::Get().BAR_HEIGHT));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, hydra::Config::Get().BAR_HEIGHT));
      status.draw();

      if(table.has_value()) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - hydra::Config::Get().BAR_HEIGHT));
        table->draw();
      }

      frame.should_show();
    }
  }
}
