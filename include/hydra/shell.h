#pragma once

#include <memory>

#include <hydra/util/callback.h>
#include <hydra/types.h>
#include <hydra/backend/sdl.h>
#include <hydra/config.h>

union SDL_Event;
namespace hydra::shell {
  struct Shell {
    using Callback = hydra::util::Callback<Option::value_t>;

    Shell(bool external_input = false);
    ~Shell();

    void stop();
    bool done();
    void run(Window& window, Callback& callback);

    void show(Table&& table, const clock_t::time_point& show_time = clock_t::now());
    void show(Search&& search);

    void show_status(std::string_view msg);
    void show_status(std::string_view msg, const clock_t::duration& duration);
    void show_error(std::string_view msg) { show_status(msg, hydra::Config::Get().ERROR_TIMEOUT); }

    bool wants_input();
    void handle_key(Key key, uint64_t timestamp_ns);

  private:
    struct Self;
    std::unique_ptr<Self> self;
  };
}
