enable_testing()
add_executable(cpptesting test.cpp)
target_link_libraries(cpptesting ${GTEST_LIB} GTest::gtest_main cassette_lib)
add_test(thetest cpptesting)
include(GoogleTest)
gtest_discover_tests(cpptesting)
