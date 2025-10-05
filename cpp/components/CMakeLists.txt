# Order matters, as the dependencies should declared & built first, right?
add_subdirectory(ipc)
add_subdirectory(tools)
add_subdirectory(fileindex)
add_subdirectory(metadata_store)
add_subdirectory(musicdb)

add_subdirectory(core)
