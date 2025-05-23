enable_testing()
add_executable(cpptesting test.cpp)
target_link_libraries(cpptesting ${GTEST_LIB} cassette_lib)
add_test(thetest cpptesting)
