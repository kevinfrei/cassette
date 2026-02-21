# Minimum version of CMake required to build this project
cmake_minimum_required(VERSION 3.28.0)

# Name of the project
project(cassette)

# Why not?
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
  # For windows/VC++, run normal exception handling & windows 10 APIs
  add_compile_options(/EHsc /W4)
  add_compile_definitions(
    _WIN32_WINNT=0x0A00
    WINVER=0x0A00
    _CRT_SECURE_NO_WARNINGS
  )
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT EditAndContinue)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
    # I wonder if RTC still works. I should use it, since I did the work:
    set(CMAKE_MSVC_RUNTIME_CHECKS "StackFrameErrorCheck;UninitializedVariable")

    add_compile_options(/Od /FS)
    add_link_options(/DEBUG:FULL /OPT:NOREF /PROFILE)
  else()
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)

    # Enable WPO explicitly, instead of relying on CMake's specific support
    add_compile_options(/GL /O2)
    add_link_options(/LTCG /OPT:REF /OPT:ICF)
  endif()
else()
  # For non-windows compilers, enable sanitizers and stuff
  add_compile_options(-Wpedantic -Wall -Wextra -pedantic)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g) # -fsanitize=address,undefined -O0 -g)
    add_link_options(-O0 -g) # -fsanitize=address,undefined -O0 -g)
  else()
    add_compile_options(-flto -O2)
    add_link_options(-flto)
  endif()
endif()

# Find Conan2 generated libraries
include(build/ConanLibImports.cmake)
enable_testing()
include(GoogleTest)

# webview isn't on conan, or vcpkg, but this is how it says to use it
include(FetchContent)
FetchContent_Declare(
  webview
  GIT_REPOSITORY
  https://github.com/webview/webview
  GIT_TAG
  0.12.0
)
FetchContent_MakeAvailable(webview)
set(WEBVIEW_LIB webview::core)

# The actual C++ project lives in the cpp/ subdirectory
add_subdirectory(cpp)
