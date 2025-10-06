add_executable(fileindex_testing fileindex_test.cpp)
target_link_libraries(fileindex_testing PUBLIC fileindex_lib ${GTEST_LIB})

gtest_discover_tests(fileindex_testing)
