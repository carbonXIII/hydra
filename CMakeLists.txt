cmake_minimum_required(VERSION 3.28)

project(hydra)

set(FETCH_TARGET_DIR ${CMAKE_BINARY_DIR}/deps
    CACHE STRING "Target directory for fetching sources by FetchContent")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON
    CACHE BOOL "Enable exporting compilation database"
    FORCE)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(ext)
add_subdirectory(src/common)
add_subdirectory(src/shell)
add_subdirectory(src/server)
add_subdirectory(test)

# Packaging
install(TARGETS hydra RUNTIME DESTINATION bin)

set(CPACK_PACKAGE_CONTACT jordan@carbondoes.dev)

set(CPACK_DEBIAN_PACKAGE_DEPENDS
  "libmiral7"
  "mir-graphics-drivers-desktop"
  "libfmt-dev"
)
list(JOIN CPACK_DEBIAN_PACKAGE_DEPENDS "," CPACK_DEBIAN_PACKAGE_DEPENDS)

include(CPack)
