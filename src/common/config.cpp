#include <hydra/config.h>

#include <hydra/backend/sdl.h>

namespace hydra {
  Config const& Config::Get() {
    static constexpr Config config;
    return config;
  }
}
