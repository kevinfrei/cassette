# Order matters, as the dependencies should declared & built first, right?
add_subdirectory(ipc)
add_subdirectory(tools)
add_subdirectory(core)

# Compile main.cpp
add_executable(my-cuark-app main.cpp)

# Link with the core_lib target build in the components subdir.
# Most of the code lives in there, so it's more easily tested.
target_link_libraries(my-cuark-app PRIVATE core_lib)
