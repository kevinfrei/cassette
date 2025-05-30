
set(GEN_HEADER ${PROJECT_SOURCE_DIR}/include/SharedConstants.hpp)

# Define a custom command to generate the header file
add_custom_command(
  PRE_BUILD 
  COMMENT
    "Generating shared constants header file"
  COMMAND
    bun gen
  DEPENDS
    ${PROJECT_SOURCE_DIR}/scripts/gencpp.ts
    ${PROJECT_SOURCE_DIR}/www/SharedConstants.ts
  OUTPUT
    ${GEN_HEADER}
)

set(
  LIB_SOURCE_FILES
  ${GEN_HEADER}
  files.cpp
  handlers.cpp
  quitting.cpp
  window.cpp
)

# Compile main.cpp
add_library(cassette_lib ${LIB_SOURCE_FILES})
add_executable(cassette main.cpp)

target_include_directories(cassette_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link the dependencies with our binary
target_link_libraries(cassette_lib PUBLIC ${BOOST_LIB} ${CROW_LIB} ${MEDIAINFO_LIB})
target_link_libraries(cassette PRIVATE cassette_lib)