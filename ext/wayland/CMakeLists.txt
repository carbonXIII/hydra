FetchContent_DeclareEx(
  hyprwayland-scanner
  GIT_REPOSITORY https://github.com/hyprwm/hyprwayland-scanner.git
  GIT_TAG        b68dab23fc922eae99306988133ee80a40b39ca5
  PATCH_COMMAND  sed -i "s/CMAKE_SOURCE_DIR/CMAKE_CURRENT_LIST_DIR/g" CMakeLists.txt
)

FetchContent_MakeAvailable(hyprwayland-scanner)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/_deps/extra-cmake-modules-src/find-modules")

function(add_wayland_client_protocol target)
  set(options "")
  set(multiValueArgs PROTOCOLS)
  set(oneValueArgs "")
  cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

  foreach(protocol ${arg_PROTOCOLS})
    get_filename_component(_protocol ${protocol} ABSOLUTE)
    cmake_path(GET _protocol STEM _protocol_name)

    set(_outputs
      "${CMAKE_CURRENT_BINARY_DIR}/${_protocol_name}.hpp"
      "${CMAKE_CURRENT_BINARY_DIR}/${_protocol_name}.cpp")
    add_custom_command(
      OUTPUT  ${_outputs}
      COMMAND $<TARGET_FILE:hyprwayland-scanner> --client ${_protocol} ${CMAKE_CURRENT_BINARY_DIR}
      DEPENDS ${_protocol} VERBATIM
    )
    target_sources(${target} PUBLIC ${_outputs})
  endforeach()
endfunction()

add_library(wayland-ext INTERFACE)
add_wayland_client_protocol(wayland-ext PROTOCOLS
  xdg-shell.xml # Required by layer-shell
  wlr-layer-shell-unstable-v1.xml)
target_include_directories(wayland-ext INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(wayland-ext INTERFACE wayland-client)
