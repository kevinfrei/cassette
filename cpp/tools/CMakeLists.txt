set(TOOLS_HEADER_FILES toolbox.h)
set(TOOLS_SRC_FILES hiddenfile.cpp)

add_library(
  tools_lib
  STATIC
  ${TOOLS_SRC_FILES}
  ${TOOLS_HEADER_FILES}
)

# Specify the include directories for consumers of this library
target_include_directories(
  tools_lib
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
