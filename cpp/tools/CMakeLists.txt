set(TOOLS_SRC_FILES hiddenfile.cpp)
add_library(tools_lib ${LIB_SOURCE_FILES})
file(GLOB TOOL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(TRANSFORM TOOL_SRC_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
file(GLOB TOOL_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../../include/tools/*.h")

# Create the static library
add_library(tools STATIC ${TOOL_SRC_FILES} ${TOOL_HEADER_FILES})

# Specify the include directories for consumers of this library
target_include_directories(tools PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Not working, but want to make a branch...
