find_package(fmt)
find_package(SDL3)

add_library(hydra-shell STATIC
  backend/sdl.cpp
  backend/imgui.cpp)

target_link_libraries(hydra-shell PRIVATE
  fmt)

target_link_libraries(hydra-shell PUBLIC imgui SDL3::SDL3) # FIXME

target_include_directories(hydra-shell PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(hydra-shell PUBLIC ${CMAKE_SOURCE_DIR}/include)
