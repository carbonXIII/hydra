cmake_minimum_required(VERSION 3.30)

project(hydra)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON
    CACHE BOOL "Enable exporting compilation database"
    FORCE)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src/server)

# Packaging
install(TARGETS hydra RUNTIME DESTINATION bin)

set(CPACK_PACKAGE_CONTACT jordan@carbondoes.dev)

set(CPACK_DEBIAN_PACKAGE_DEPENDS
  "libmiral5t64"
  "mir-graphics-drivers-desktop"
  "libfmt-dev"
)
list(JOIN CPACK_DEBIAN_PACKAGE_DEPENDS "," CPACK_DEBIAN_PACKAGE_DEPENDS)

include(CPack)
