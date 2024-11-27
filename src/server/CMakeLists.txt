include(FindPkgConfig)
pkg_check_modules(miral REQUIRED miral IMPORTED_TARGET)
find_package(fmt)

add_executable(hydra main.cpp)

target_link_libraries(hydra PRIVATE fmt PkgConfig::miral)
