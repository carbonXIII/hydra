#include <fmt/core.h>

#include <chrono>

#include <imgui/imgui.h>

#include <hydra/config.h>
#include <hydra/util/util.h>
#include <hydra/backend/sdl.h>
#ifndef NO_WAYLAND_EXTENSIONS
#include <hydra/backend/layer_window.h>
#endif

#include <shell/backend/imgui.h>
#include <shell/widget.h>

using namespace hydra::shell;

enum Event: hydra::Option::value_t {
  ERROR = -1,
  QUIT,
  COUNTER,
  HELLO,
  SEARCH,
};

static constexpr auto corpus = std::array {
  "Lorem", "ipsum", "dolor",
  "sit", "amet", "consectetur",
  "adipiscing", "elit", "Nam",
  "imperdiet",
};

int main() {
  SDLContext context;

#ifndef NO_WAYLAND_EXTENSIONS
  using Window = hydra::shell::LayerWindow;
#else
  using Window = hydra::shell::Window;
#endif

  StatusLine status;
  std::optional<Prompt> prompt;

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
        if(prompt.has_value()) {
          handle_key(*prompt, hydra::Key::Raw(e.key.raw));

          if(auto res = try_result(*prompt)) {
            bool is_table = std::holds_alternative<TablePrompt>(*prompt);
            prompt.reset();

            if(is_table) {
              switch(Event(*res)) {
                case ERROR:
                  status.show("Undefined");
                  break;
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
                case SEARCH:
                  {
                    hydra::Search opts;
                    for(unsigned idx = 0; auto word: corpus) {
                      opts.push_back(hydra::Option{idx++, word});
                    }
                    prompt.emplace(SearchPrompt{opts});
                  }
                  break;
                default:
                  break;
              }
            } else {
              if(auto idx = *res; idx >= 0 && idx < corpus.size()) {
                status.show(fmt::format("search result: {}", corpus[idx]),
                            std::chrono::milliseconds(5000));
              }
            }
          }
        } else {
          switch(e.key.key) {
            case SDLK_SPACE:
              prompt.emplace(TablePrompt{
                  hydra::Table{
                    std::pair{hydra::Key::Scancode(SDL_SCANCODE_H), hydra::Option{HELLO, "Hello"}},
                    std::pair{hydra::Key::Scancode(SDL_SCANCODE_C), hydra::Option{COUNTER, "Counter++"}},
                    std::pair{hydra::Key::Scancode(SDL_SCANCODE_Q), hydra::Option{QUIT, "Quit"}},
                    std::pair{hydra::Key::Scancode(SDL_SCANCODE_SPACE), hydra::Option{SEARCH, "Search"}},
                  }
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

      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - hydra::Config::Get().BAR_HEIGHT));
      if(prompt.has_value() && should_draw(*prompt)) {
        draw(*prompt);
      }

      frame.should_show();
    }
  }
}
