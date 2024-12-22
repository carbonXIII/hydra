#pragma once

#include <SDL3/SDL_keyboard.h>

#include <chrono>
#include <string>

#include <hydra/backend/scancode.h>

namespace hydra {
  using clock_t = std::chrono::system_clock;

  struct Key {
    constexpr static Key Raw(uint32_t raw) {
      return { raw };
    }

    constexpr static Key Scancode(SDL_Scancode sc) {
      if(sc > scancode_to_raw.size()) {
        sc = SDL_SCANCODE_UNKNOWN;
      }
      return { scancode_to_raw[sc] };
    }

    static Key Keycode(SDL_Keycode kc) {
      return Scancode(SDL_GetScancodeFromKey(kc, nullptr));
    }

    static Key String(const char* name) {
      return Scancode(SDL_GetScancodeFromName(name));
    }

    constexpr uint32_t get() const {
      return raw;
    }

    constexpr SDL_Scancode scancode() const {
      if(raw > raw_to_scancode.size()) {
        return SDL_SCANCODE_UNKNOWN;
      }
      return raw_to_scancode[raw];
    }

    constexpr SDL_Keycode keycode() const {
      return SDL_GetKeyFromScancode(scancode(), SDL_KMOD_NONE, true);
    }

    friend std::string to_string(Key const& key) {
      switch(auto code = key.keycode()) {
        case SDLK_ESCAPE:
          return "ESC";
        default:
          return SDL_GetKeyName(code);
      }
    }

    auto operator<=>(Key const&) const = default;

  protected:
    constexpr Key(uint32_t raw): raw(raw) {}

    uint32_t raw;
  };

  struct Option {
    using value_t = int64_t;
    value_t value;
    std::string name;
  };

  using Table = std::vector<std::pair<Key, Option>>;
  using Search = std::vector<Option>;
}
