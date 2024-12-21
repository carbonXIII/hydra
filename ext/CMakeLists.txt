include(FetchContent)
function(FetchContent_DeclareEx contentName)
  message("FetchContent source ${FETCH_TARGET_DIR}/${contentName}")
  FetchContent_Declare(${ARGV} SOURCE_DIR ${FETCH_TARGET_DIR}/${contentName})
endfunction()

set(SDL_STATIC ON CACHE BOOL "Build SDL static target" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Don't build SDL shared target" FORCE)
FetchContent_DeclareEx(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        eb3fc0684c9957de108df5b338307418b0130c3b
  OVERRIDE_FIND_PACKAGE
)

FetchContent_DeclareEx(
  rapidfuzz
  GIT_REPOSITORY https://github.com/rapidfuzz/rapidfuzz-cpp.git
  GIT_TAG        c6a3ac87c42ddf52f502dc3ed7001c8c2cefb900
  OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  OVERRIDE_FIND_PACKAGE
)

add_subdirectory(imgui EXCLUDE_FROM_ALL)
add_subdirectory(wayland EXCLUDE_FROM_ALL)
