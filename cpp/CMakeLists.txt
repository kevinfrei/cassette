# This is the "outer" CMakeLists.txt file for the whole project

add_subdirectory(components)

# Compile main.cpp
add_executable(cassette main.cpp)

# Link with the core_lib target build in the components subdir.
# Most of the code lives in there, so it's more easily tested.
target_link_libraries(cassette PRIVATE core_lib ${CROW_LIB})

add_subdirectory(test)
