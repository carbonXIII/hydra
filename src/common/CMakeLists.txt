find_package(SDL3)
find_package(Fontconfig)

add_library(hydra-common STATIC
  config.cpp)

target_include_directories(hydra-common PUBLIC ${CMAKE_SOURCE_DIR}/include/)
target_link_libraries(hydra-common PUBLIC SDL3::SDL3-static fontconfig)
target_link_libraries(hydra-common PRIVATE imgui)
