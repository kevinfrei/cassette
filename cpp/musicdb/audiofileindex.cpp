#include <filesystem>
#include <iostream>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "audiofileindex.h"

namespace fs = std::filesystem;

namespace afi {

audio_file_index::audio_file_index(const std::filesystem::path& _loc,
                                   std::size_t _hash)
    : hash(_hash), loc(std::filesystem::canonical(_loc)) {
  if (hash == 0) {
    // Compute hash based on the location, if not provided.
    std::hash<std::string> hasher;
    hash = hasher(loc.generic_string());
  }
}

audio_file_index* audio_file_index::create(const std::filesystem::path& loc) {
  return new audio_file_index(loc);
}

} // namespace afi
