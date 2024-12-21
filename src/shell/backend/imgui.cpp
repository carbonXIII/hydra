#include <backend/imgui.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl3.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <hydra/backend/sdl.h>

namespace hydra::shell {
  static const char* glsl_version = "#version 130";

  FrameContext::FrameContext(Window* window): window(window) {
    auto gl_context = window->gl_context();
    SDL_GL_MakeCurrent(window->get(), gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_ShowWindow(window->get());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiLoadConfig(ImGui::GetStyle());
    ImGui_ImplSDL3_InitForOpenGL(window->get(), gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
  }

  FrameContext::~FrameContext() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
  }

  FrameContext::Guard::Guard(FrameContext* parent)
    : parent(parent), shown(false) {}

  FrameContext::Guard::~Guard() {
    parent->swap_frame(shown);
  }

  FrameContext::Guard FrameContext::start_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    return Guard(this);
  }

  void FrameContext::handle_event(const SDL_Event& e) {
    ImGui_ImplSDL3_ProcessEvent(&e);
  }

  void FrameContext::swap_frame(bool shown) {
    ImGui::EndFrame();
    ImGui::Render();

    // render
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.0f);
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    if(shown) {
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_GL_SwapWindow(window->get());
  }
}
