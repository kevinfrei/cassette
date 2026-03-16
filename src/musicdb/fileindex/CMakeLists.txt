# The fileindex library. It's used to maintain a list of files on disk.
# It scans the filesystem, and keeps track of changes.

add_library(fileindex_lib STATIC fileindex.cpp)
target_include_directories(fileindex_lib PUBLIC include)
# target_include_directories(fileindex_lib PRIVATE impl)

# Link the dependencies with our binary
target_link_libraries(fileindex_lib PUBLIC ipc_lib tools_lib)

add_subdirectory(test)
