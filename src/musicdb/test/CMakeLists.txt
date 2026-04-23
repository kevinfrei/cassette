add_executable(musicdb_testing musicdb_test.cpp)
target_link_libraries(musicdb_testing PUBLIC musicdb_lib ${GTEST_LIB})
gtest_discover_tests(musicdb_testing)
