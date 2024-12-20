#include <hydra/backend/sdl.h>

#include <fmt/format.h>

namespace hydra::shell {
  static void error_check(auto value) {
    if(!value) {
      throw std::runtime_error(fmt::format("SDL: {}", SDL_GetError()));
    }
  }

  SDLContext::SDLContext() {
    error_check(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

    // GL(SL) version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  }

  SDLContext::~SDLContext() {
    SDL_Quit();
  }

  Window::Window(const SDLContext&, Properties&& props) {
    window.reset(SDL_CreateWindowWithProperties(props));
  }

  SDL_Window* Window::get() {
    return window.get();
  }

  SDL_GLContext Window::gl_context() {
    if(!_gl_context) {
      _gl_context.reset(SDL_GL_CreateContext(get()));
    }

    return _gl_context.get();
  }

  Window::operator SDL_Window*() {
    return get();
  }

  Properties::Properties(): props(SDL_CreateProperties()) {}

  Properties::Properties(Properties&& o): props(std::exchange(o.props, {})) {}

  Properties::~Properties() {
    if(props) {
      SDL_DestroyProperties(props);
    }
  }

  Properties::operator SDL_PropertiesID() {
    return props;
  }
}
