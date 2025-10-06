add_executable(tools_testing text_norm_test.cpp)
target_link_libraries(tools_testing PUBLIC tools_lib ${GTEST_LIB})

gtest_discover_tests(tools_testing)
