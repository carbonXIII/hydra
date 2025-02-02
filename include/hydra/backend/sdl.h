#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_properties.h>

#include <memory>

struct wl_display;
namespace hydra::shell {
  struct SDLContext {
    SDLContext(wl_display* display = nullptr);
    ~SDLContext();
  };

  struct Window {
    struct Properties;

    Window(SDLContext const&, Properties&& props);

    SDL_Window* get();
    SDL_GLContext gl_context();

    operator SDL_Window*();
    Window(Window const&) = delete;

    virtual bool has_focus();
    virtual void set_focusable(bool val);
    virtual void raise();
    virtual bool should_hide() { return false; }

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
    std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, SDL_GLContext_Deleter> _gl_context;
  };

  struct Window::Properties {
    Properties();
    Properties(Properties const&) = delete;
    Properties(Properties&& o);
    ~Properties();

    operator SDL_PropertiesID();

    static Properties FromConfig();

  private:
    SDL_PropertiesID props;
  };
}
