include(FindPkgConfig)
pkg_check_modules(miral REQUIRED miral IMPORTED_TARGET)
pkg_search_module(giomm REQUIRED "giomm-2.4" IMPORTED_TARGET)
find_package(fmt)

add_executable(hydra
  shell_launcher.cpp
  window_manager.cpp
  main.cpp)

target_link_libraries(hydra PRIVATE
  hydra-common
  hydra-shell
  fmt
  PkgConfig::miral
  PkgConfig::giomm)

target_include_directories(hydra PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
