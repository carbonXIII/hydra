#include <fmt/core.h>

#include <chrono>
#include <thread>

#include <imgui/imgui.h>

#include <hydra/config.h>
#include <hydra/util/util.h>
#include <hydra/backend/sdl.h>
#ifndef NO_WAYLAND_EXTENSIONS
#include <hydra/backend/layer_window.h>
#endif

#include <hydra/shell.h>

using namespace hydra::shell;

enum Event: hydra::Option::value_t {
  ERROR = -1,
  QUIT,
  COUNTER,
  HELLO,
  SEARCH,
};

enum class State {
  IDLE,
  TABLE,
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

  Shell shell;

  Window window(context, Window::Properties::FromConfig());

  std::mutex state_lock;
  State state = State::IDLE;
  int test_counter = 0;
  auto cb = hydra::util::Callback::Create([&](auto res) {
    std::lock_guard g{state_lock};

    switch(std::exchange(state, State::IDLE)) {
      case State::IDLE:
        throw std::runtime_error("unreachable");
        break;
      case State::TABLE:
        switch(Event(res)) {
          case ERROR:
            shell.show_error("Undefined");
            break;
          case QUIT:
            shell.stop();
            break;
          case COUNTER:
            ++test_counter;
            shell.show_error(fmt::format("Counter: {}", test_counter));
            break;
          case HELLO:
            shell.show_status("hello status line", std::chrono::milliseconds(5000));
            break;
          case SEARCH:
            {
              state = State::SEARCH;
              shell.show([](){
                hydra::Search opts;
                for(unsigned idx = 0; auto word: corpus) {
                  opts.push_back(hydra::Option{idx++, word});
                }
                return opts;
              }());
            }
            break;
          default:
            break;
        }
        break;

      case State::SEARCH:
        if(res >= 0 && res < corpus.size()) {
          shell.show_status(fmt::format("search result: {}", corpus[res]),
                            std::chrono::milliseconds(5000));
        }
        break;
    }
  });

  // Simulate another process triggering initial state change
  std::jthread external([&](){
    while(!shell.done()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      {
        std::lock_guard g{state_lock};
        if(state == State::IDLE) {
          state = State::TABLE;
          shell.show(hydra::Table{
              std::pair{hydra::Key::Scancode(SDL_SCANCODE_H), hydra::Option{HELLO, "Hello"}},
              std::pair{hydra::Key::Scancode(SDL_SCANCODE_C), hydra::Option{COUNTER, "Counter++"}},
              std::pair{hydra::Key::Scancode(SDL_SCANCODE_Q), hydra::Option{QUIT, "Quit"}},
              std::pair{hydra::Key::Scancode(SDL_SCANCODE_SPACE), hydra::Option{SEARCH, "Search"}},
            });
        }
      }
    }
  });

  shell.run(window, cb);

  return 0;
}
