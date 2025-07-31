#include <filesystem>
#include <iostream>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "audiofileindex.h"

namespace fs = std::filesystem;

namespace afi {

audio_file_index::audio_file_index(const std::filesystem::path& loc)
    : loc(loc) {}

audio_file_index* audio_file_index::create(const std::filesystem::path& loc) {
  return new audio_file_index(loc);
}

} // namespace afi
