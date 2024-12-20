include(FetchContent)
function(FetchContent_DeclareEx contentName)
  message("FetchContent source ${FETCH_TARGET_DIR}/${contentName}")
  FetchContent_Declare(${ARGV} SOURCE_DIR ${FETCH_TARGET_DIR}/${contentName})
endfunction()

FetchContent_DeclareEx(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        eb3fc0684c9957de108df5b338307418b0130c3b
  OVERRIDE_FIND_PACKAGE
)

add_subdirectory(imgui EXCLUDE_FROM_ALL)
