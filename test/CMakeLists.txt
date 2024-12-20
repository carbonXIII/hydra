add_executable(shell-example shell/example.cpp)
target_link_libraries(shell-example hydra-shell)
target_include_directories(shell-example PRIVATE ${CMAKE_SOURCE_DIR}/src)
