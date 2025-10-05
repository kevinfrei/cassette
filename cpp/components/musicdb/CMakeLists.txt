add_library(musicdb_lib STATIC musicdb.cpp)
target_include_directories(musicdb_lib PUBLIC include)
# target_include_directories(musicdb_lib PRIVATE impl)

# Link the dependencies with our binary
target_link_libraries(
  musicdb_lib
  PUBLIC
    ipc_lib ftools_lib fileindex_lib metadata_store_lib
)
