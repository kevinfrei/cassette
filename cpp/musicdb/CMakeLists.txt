set(
  MUSICDB_SRC_FILES
  audiofileindex.cpp
  musicdb.cpp
)

add_library(
  musicdb_lib
  OBJECT
  ${MUSICDB_SRC_FILES}
)

target_link_libraries(
  musicdb_lib
  PUBLIC
  ${CROW_LIB}
  ${MEDIAINFO_LIB}
)

# Specify the include directories for consumers of this library
target_include_directories(
  musicdb_lib
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
