# Minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.28)

# Name of the project
project(cassette)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
if(MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc /D_WIN32_WINNT=0x0A00 /DWINVER=0x0A00")
endif(MSVC)

# Find Conan2 generated libraries
include(cmake/ConanLibImports.cmake)

add_subdirectory(cpp)
add_subdirectory(test)
