add_executable(cassette_testing config_test.cpp)
target_link_libraries(
  cassette_testing
  PUBLIC
    ${GTEST_LIB}
    core_lib
    ${BOOST_LIB}
    ${CROW_LIB}
)

gtest_discover_tests(cassette_testing)
