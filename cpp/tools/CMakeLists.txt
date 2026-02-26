# A few little tools for everyone to use

add_library(tools_lib STATIC base64.cpp file_tools.cpp text_tools.cpp)
target_include_directories(tools_lib PUBLIC include)
# target_include_directories(tools_lib PRIVATE impl)

# Link the dependencies with our binary
# target_link_libraries(tools_lib PUBLIC ...)

add_subdirectory(test)
