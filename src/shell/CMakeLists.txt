find_package(fmt)
find_package(SDL3)

add_library(hydra-shell STATIC
  backend/sdl.cpp
  backend/layer_shell_wrapper.cpp
  backend/layer_window.cpp
  backend/imgui.cpp)

target_link_libraries(hydra-shell PRIVATE
  hydra-common
  wayland-ext
  fmt)

target_link_libraries(hydra-shell PUBLIC imgui) # FIXME
target_include_directories(hydra-shell PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
