#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include <hydra/backend/sdl.h>
#include <hydra/backend/layer_shell_enum.h>

namespace hydra::shell {
  struct LayerWindow: public Window {
    LayerWindow(SDLContext const&, Window::Properties&& props);
    ~LayerWindow();

    void set_focusable(bool val) override;
    void set_exclusive(bool val);
    bool should_hide() override;

  private:
    struct Self;
    std::unique_ptr<Self> self;
  };
}
