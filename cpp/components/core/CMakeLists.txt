# The core library of cassette, implementing the main logic.
# This is basically everything except the main entry point and the tests.

add_library(
  core_lib
  STATIC
    api.cpp
    config.cpp
    files.cpp
    from_json.cpp
    handlers.cpp
    quitting.cpp
    setup.cpp
    tools.cpp
    tunes.cpp
    websocket.cpp
    window.cpp
)
target_include_directories(core_lib PUBLIC include)
target_include_directories(core_lib PRIVATE impl)

# Link the dependencies with our binary
target_link_libraries(
  core_lib
  PUBLIC
    ipc_lib tools_lib musicdb_lib ${CROW_LIB} ${PFD_LIB}
)
