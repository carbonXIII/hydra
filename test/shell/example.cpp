#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <fmt/core.h>

#include <hydra/backend/sdl.h>

using namespace hydra::shell;

auto get_window_props() {
  Window::Properties props;
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, true);
  SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 640);
  SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 480);
  return props;
}

int main() {
  SDLContext context;
  Window window(context, get_window_props());

  SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

  SDL_ShowWindow(window);
  bool done = false;
  while(!done) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_EVENT_QUIT) {
        done = true;
      } else if(e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_Q) {
        done = true;
      }
    }

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }
}
