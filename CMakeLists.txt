# Minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.15)

# Name of the project
project(cassette)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Gotta have at least 1 find_package for the conan provider to trigger properly
# I wonder what I might be able to do to address this...

# Find Conan2 generated libraries
include(cmake/ConanLibImports.cmake)

set(
    SOURCE_FILES
    cpp/main.cpp
    cpp/window.cpp
)

# Compile main.cpp
add_executable(cassette ${SOURCE_FILES})

target_include_directories(cassette PUBLIC include)

# Link the dependencies with our binary
target_link_libraries(cassette PUBLIC ${CROW_LIB} ${MEDIAINFO_LIB})