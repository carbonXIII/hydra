#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include <hydra/backend/sdl.h>
#include <hydra/backend/layer_shell_enum.h>

namespace hydra::shell {
  struct LayerWindow: public Window {
    LayerWindow(const SDLContext&, Window::Properties&& props);
    ~LayerWindow();

  private:
    struct Self;
    std::unique_ptr<Self> self;
  };
}
