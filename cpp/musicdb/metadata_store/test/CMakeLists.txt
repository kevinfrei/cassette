add_executable(metadata_testing metadata_test.cpp)
target_link_libraries(metadata_testing PUBLIC metadata_lib ${GTEST_LIB})

gtest_discover_tests(metadata_testing)
