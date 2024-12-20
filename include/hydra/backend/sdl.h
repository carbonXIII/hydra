#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_properties.h>

#include <wayland-client-protocol.h>

#include <memory>
#include <utility>

namespace hydra::shell {
  struct SDLContext {
    SDLContext();
    ~SDLContext();
  };

  struct Window {
    struct Properties;

    Window(const SDLContext&, Properties&& props);

    SDL_Window* get();
    SDL_GLContext gl_context();

    operator SDL_Window*();

    Window(const Window&) = delete;

  private:
    struct SDL_Window_Deleter {
      void operator()(SDL_Window* win) {
        SDL_DestroyWindow(win);
      }
    };

    struct SDL_GLContext_Deleter {
      void operator()(SDL_GLContext context) {
        SDL_GL_DestroyContext(context);
      }
    };

    std::unique_ptr<SDL_Window, SDL_Window_Deleter> window;
    std::unique_ptr<std::remove_pointer<SDL_GLContext>::type, SDL_GLContext_Deleter> _gl_context;
  };

  struct Window::Properties {
    Properties();
    Properties(const Properties&) = delete;
    Properties(Properties&& o);
    ~Properties();

    operator SDL_PropertiesID();

  private:
    SDL_PropertiesID props;
  };
}
