enable_testing()

add_executable(
  cassette_testing
  json_test.cpp
  config_test.cpp
  fileindex_test.cpp
)
target_link_libraries(
  cassette_testing
  PUBLIC
    ${GTEST_LIB}
    core_lib
    ipc_lib
    tools_lib
    musicdb_lib
    ${BOOST_LIB}
    ${CROW_LIB}
    GTest::gtest_main
)
add_test(thetest cassette_testing)

include(GoogleTest)
gtest_discover_tests(cassette_testing)
