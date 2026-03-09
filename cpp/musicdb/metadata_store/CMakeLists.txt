add_library(metadata_store_lib STATIC metadata.cpp)
target_include_directories(metadata_store_lib PUBLIC include)
# target_include_directories(metadata_store_lib PRIVATE impl)

# Link the dependencies with our binary
target_link_libraries(
  metadata_store_lib
  PUBLIC
    ipc_lib tools_lib ${BOOST_LIB} ${TAGLIB_LIB}
)
