#pragma once

#include <SDL3/SDL_events.h>

#include <utility>

struct ImGuiStyle;
namespace hydra::shell {
  struct Window;

  void ImGuiLoadConfig(ImGuiStyle& style);

  struct FrameContext {
    FrameContext(Window* window);
    ~FrameContext();

    struct [[nodiscard]] Guard {
      ~Guard();
      void should_show() { shown = true; }

    private:
      friend struct FrameContext;
      Guard(FrameContext* parent);
      Guard(const Guard&) = delete;
      FrameContext* parent;
      bool shown;
    };

    void handle_events(auto&& event_cb) {
      SDL_Event e;
      while(SDL_PollEvent(&e)) {
        handle_event(e);
        std::forward<decltype(event_cb)>(event_cb)(e);
      }
    }

    Guard start_frame();

  protected:
    friend struct Guard;
    void swap_frame(bool shown);

  private:
    void handle_event(const SDL_Event& e);

    Window* window;
  };
}
