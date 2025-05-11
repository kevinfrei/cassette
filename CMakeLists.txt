# Minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.15)

# Name of the project
project(cassette)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Find Conan2 generated libraries
include(ConanLibImports.cmake)

set(
    SOURCE_FILES
    cpp/main.cpp
)

# Compile main.cpp
add_executable(cassette ${SOURCE_FILES})

# Link the dependencies with our binary
target_link_libraries(cassette PUBLIC ${CROW_LIB})