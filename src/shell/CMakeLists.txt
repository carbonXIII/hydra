find_package(fmt)
find_package(SDL3)
find_package(rapidfuzz)

add_library(hydra-shell STATIC
  shell.cpp
  search.cpp
  backend/sdl.cpp
  backend/layer_shell_wrapper.cpp
  backend/foreign_toplevel_wrapper.cpp
  backend/layer_window.cpp
  backend/imgui.cpp
  backend/imgui_config.cpp)

target_link_libraries(hydra-shell PRIVATE
  hydra-common
  wayland-ext
  rapidfuzz::rapidfuzz
  fmt)

target_link_libraries(hydra-shell PUBLIC imgui) # FIXME
target_include_directories(hydra-shell PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
