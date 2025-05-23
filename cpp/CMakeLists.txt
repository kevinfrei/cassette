set(
    LIB_SOURCE_FILES
    files.cpp
    handlers.cpp
    quitting.cpp
    window.cpp
)

# Compile main.cpp
add_library(cassette_lib ${LIB_SOURCE_FILES})
add_executable(cassette main.cpp)

target_include_directories(cassette_lib PUBLIC ../include)

# Link the dependencies with our binary
target_link_libraries(cassette_lib PUBLIC ${BOOST_LIB} ${CROW_LIB} ${MEDIAINFO_LIB})
target_link_libraries(cassette PRIVATE cassette_lib)