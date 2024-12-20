set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FetchContent_DeclareEx(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        f401021d5a5d56fe2304056c391e78f81c8d4b8f
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_DOWNLOAD_DIR "${FETCH_TARGET_DIR}/imgui")

file(GLOB _IMGUI_SOURCE "${IMGUI_DOWNLOAD_DIR}/*.cpp")
file(GLOB _IMGUI_HEADERS "${IMGUI_DOWNLOAD_DIR}/*.h")
list(APPEND _IMGUI_SOURCE ${_IMGUI_HEADERS})
list(APPEND _IMGUI_SOURCE
  ${IMGUI_DOWNLOAD_DIR}/misc/cpp/imgui_stdlib.h
  ${IMGUI_DOWNLOAD_DIR}/misc/cpp/imgui_stdlib.cpp
  ${IMGUI_DOWNLOAD_DIR}/backends/imgui_impl_opengl3.h
  ${IMGUI_DOWNLOAD_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DOWNLOAD_DIR}/backends/imgui_impl_opengl3_loader.h
  ${IMGUI_DOWNLOAD_DIR}/backends/imgui_impl_sdl3.h
  ${IMGUI_DOWNLOAD_DIR}/backends/imgui_impl_sdl3.cpp)

set(IMGUI_SOURCE "")
set(IMGUI_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/imgui")

foreach(_SOURCE ${_IMGUI_SOURCE})
  message("Moving ${_SOURCE}")

  cmake_path(GET _SOURCE FILENAME DEST)
  set(DEST "${IMGUI_SOURCE_DIR}/${DEST}")
  list(APPEND IMGUI_SOURCE ${DEST})

  configure_file(${_SOURCE} ${DEST} COPYONLY)
endforeach()

find_package(SDL3)
find_package(OpenGL)

add_library(imgui STATIC ${IMGUI_SOURCE})
target_link_libraries(imgui PUBLIC SDL3::SDL3 OpenGL::OpenGL OpenGL::EGL)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
