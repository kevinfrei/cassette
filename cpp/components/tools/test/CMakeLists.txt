enable_testing()

add_executable(
  tools_testing
  text_norm_test.cpp
)
target_link_libraries(
  tools_testing
  PUBLIC
    # ${GTEST_LIB}
    tools_lib
    GTest::gtest_main
)
add_test(thetest tools_testing)

include(GoogleTest)
gtest_discover_tests(tools_testing)
