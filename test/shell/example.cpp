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
#include <hydra/util/state_machine_impl.h>

using namespace hydra::shell;

static constexpr auto corpus = std::array {
  "Lorem", "ipsum", "dolor",
  "sit", "amet", "consectetur",
  "adipiscing", "elit", "Nam",
  "imperdiet",
};

struct State {
  enum States: std::size_t {
    IDLE,
    TABLE,
    SEARCH,
  };

  enum class Event: hydra::Option::value_t {
    ERROR = -1,
    QUIT,
    COUNTER,
    HELLO,
    SEARCH,
  };


  auto idle() {
    return [](auto res) -> std::size_t {
      throw std::runtime_error("Unreachable");
    };
  }

  auto table() {
    shell->show(hydra::Table{
        std::pair{hydra::Key::Scancode(SDL_SCANCODE_H), hydra::Option{std::size_t(Event::HELLO), "Hello"}},
        std::pair{hydra::Key::Scancode(SDL_SCANCODE_C), hydra::Option{std::size_t(Event::COUNTER), "Counter++"}},
        std::pair{hydra::Key::Scancode(SDL_SCANCODE_Q), hydra::Option{std::size_t(Event::QUIT), "Quit"}},
        std::pair{hydra::Key::Scancode(SDL_SCANCODE_SPACE), hydra::Option{std::size_t(Event::SEARCH), "Search"}},
      });

    return [this](auto res) -> std::size_t {
      switch(Event(res)) {
        case Event::ERROR:
          shell->show_error("Undefined");
          break;
        case Event::QUIT:
          shell->stop();
          break;
        case Event::COUNTER:
          ++test_counter;
          shell->show_error(fmt::format("Counter: {}", test_counter));
          break;
        case Event::HELLO:
          shell->show_status("hello status line", std::chrono::milliseconds(5000));
          break;
        case Event::SEARCH:
          return States::SEARCH;
        default:
          break;
      }

      return States::IDLE;
    };
  }

  auto search() {
    shell->show([](){
      hydra::Search opts;
      for(unsigned idx = 0; auto word: corpus) {
        opts.push_back(hydra::Option{idx++, word});
      }
      return opts;
    }());

    return [this](auto res) -> std::size_t {
      if(res >= 0 && res < corpus.size()) {
        shell->show_status(fmt::format("search result: {}", corpus[res]),
                           std::chrono::milliseconds(5000));
      }
      return States::IDLE;
    };
  }

  Shell* shell;
  int test_counter;
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

  State state{&shell};
  auto sm = hydra::util::CreateStateMachine<
    hydra::Option::value_t,
    State,
    hydra::util::State{ State::IDLE, &State::idle },
    hydra::util::State{ State::TABLE, &State::table },
    hydra::util::State{ State::SEARCH, &State::search }
    >(&state);

  // Simulate another process triggering initial state change
  std::jthread external([&](){
    while(!shell.done()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      sm->lock([](std::size_t state) -> std::size_t {
        if(state == State::IDLE) {
          return State::TABLE;
        }
        return -1;
      });
    }
  });

  shell.run(window, *sm);

  return 0;
}
