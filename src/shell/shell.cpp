#include <hydra/shell.h>

#include <SDL3/SDL_events.h>
#include <imgui/imgui.h>

#include <chrono>
#include <optional>
#include <queue>
#include <condition_variable>

#include <hydra/config.h>
#include <widget.h>
#include <backend/imgui.h>

namespace hydra::shell {
  struct Shell::Self {
    bool is_done = false;
    bool last_active = false;
    bool external_input = false;

    clock_t::time_point activation_time;
    clock_t::time_point last_draw_time;

    StatusLine status;
    std::optional<Prompt> cur_prompt;

    std::mutex buffer_lock;
    std::condition_variable buffer_cv;
    std::queue<std::pair<Key, uint64_t>> buffer;


    void stop() {
      is_done = true;
      {
        std::lock_guard g{buffer_lock};
        buffer_cv.notify_all();
      }
    }

    void handle_event(const SDL_Event& e) {
      if(e.type == SDL_EVENT_QUIT) {
        is_done = true;
      } else if(!external_input && e.type == SDL_EVENT_KEY_DOWN && !e.key.repeat) {
        auto key = Key::Scancode(e.key.scancode);
        auto timestamp = e.key.timestamp;
        handle_key(key, timestamp);
      }
    }

    void handle_key(Key key, uint64_t timestamp_ns) {
      std::lock_guard g{buffer_lock};

      const auto timeout = Config::Get().buffer_timeout<std::chrono::nanoseconds>();
      while(buffer.size() && timestamp_ns - buffer.front().second > timeout) {
        buffer.pop();
      }

      buffer.push({key, timestamp_ns});
      buffer_cv.notify_all();
    }

    void consume_keys(clock_t::time_point deadline) {
      std::unique_lock g{buffer_lock};

      if(external_input && buffer.empty() && !cur_prompt.has_value()) {
        buffer_cv.wait_until(g, deadline, [this]() -> bool {
          return buffer.size() || cur_prompt.has_value() || is_done;
        });
      }

      while(buffer.size() && cur_prompt.has_value() && !try_result(*cur_prompt)) {
        ::hydra::shell::handle_key(*cur_prompt, buffer.front().first);
        buffer.pop();
      }
    }

    void show(auto&& prompt) {
      if(cur_prompt.has_value()) {
        return;
      }

      cur_prompt.emplace(std::forward<decltype(prompt)>(prompt));
      activation_time = clock_t::now();
    }

    void draw() {
      ImGuiIO& io = ImGui::GetIO();
      ImGui::SetNextWindowPos(ImVec2(0.f, io.DisplaySize.y - Config::Get().BAR_HEIGHT));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, Config::Get().BAR_HEIGHT));
      status.draw();

      if(cur_prompt.has_value() && should_draw(*cur_prompt)) {
        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - Config::Get().BAR_HEIGHT));
        ::hydra::shell::draw(cur_prompt.value());
      }
    }

    void reset() {
      cur_prompt.reset();
    }

    std::optional<Option::value_t> pop_result() {
      if(!cur_prompt.has_value()) return std::nullopt;

      if(auto res = try_result(cur_prompt.value())) {
        reset();
        return res;
      }

      return std::nullopt;
    }

    struct FrameInfo {
      bool active;
      bool repeat;
      clock_t::duration age;
    };

    FrameInfo next_frame() {
      bool active = cur_prompt.has_value();
      bool last_active = std::exchange(this->last_active, active);

      return FrameInfo {
        .active = active,
        .repeat = active == last_active,
        .age = clock_t::now() - activation_time
      };
    }

    std::optional<Option::value_t> frame(Window& window, FrameContext& fc) {
      fc.handle_events([this](const SDL_Event& e){
        handle_event(e);
      });

      consume_keys(last_draw_time + Config::Get().FRAME_TIMEOUT);

      if(auto res = pop_result()) {
        return res;
      }

      auto frame_guard = fc.start_frame();

      auto [active, repeat, age] = next_frame();
      if(!repeat) {
        window.set_focusable(active);
        if(active) {
          window.raise();
        }
      }

      if(active && !window.has_focus() && age > Config::Get().FOCUS_TIMEOUT) {
        reset();
        return -1;
      }

      if(cur_prompt.has_value() || !window.should_hide()) {
        draw();
        frame_guard.should_show();
        last_draw_time = clock_t::now();
        return pop_result();
      }

      return std::nullopt;
    }
  };

  Shell::Shell(bool external_input): self(std::make_unique<Shell::Self>()) {
    self->external_input = external_input;
  }

  Shell::~Shell() {}

  void Shell::stop() { self->stop(); }
  bool Shell::done() { return self->is_done; }

  void Shell::run(Window& window, Callback& cb) {
    self->is_done = false;

    FrameContext fc(&window);
    while(!done()) {
      if(auto res = self->frame(window, fc)) {
        cb(res.value());
      }
    }
  }

  void Shell::show(Search&& search) {
    self->show(SearchPrompt{std::forward<decltype(search)>(search)});
  }

  void Shell::show(Table&& table, const clock_t::time_point& show_time) {
    self->show(TablePrompt{std::forward<decltype(table)>(table), show_time});
  }

  void Shell::show_status(std::string_view status) {
    self->status.show(status);
  }

  void Shell::show_status(std::string_view status, const clock_t::duration& duration) {
    self->status.show(status, duration);
  }

  bool Shell::wants_input() {
    return self->cur_prompt.has_value();
  }

  void Shell::handle_key(Key key, uint64_t timestamp_ns) {
    return self->handle_key(key, timestamp_ns);
  }
}
