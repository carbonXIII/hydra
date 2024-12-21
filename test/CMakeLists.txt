add_executable(shell-example shell/example.cpp)
target_link_libraries(shell-example hydra-shell)
target_include_directories(shell-example PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(shell-example PRIVATE ${CMAKE_SOURCE_DIR}/src)

enable_testing()
include(GoogleTest)
find_package(googletest)

function(add_test name source)
  add_executable(${name} ${source})
  target_link_libraries(${name}
    hydra-common
    GTest::gtest_main
    fmt)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src/)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include/)
  gtest_discover_tests(${name})
endfunction()

add_test(state_machine util/state_machine.cpp)
add_test(trie util/trie.cpp)
add_test(workspace_info server/workspace_info.cpp)
