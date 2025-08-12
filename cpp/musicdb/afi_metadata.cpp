#include <filesystem>
#include <optional>
#include <string>

#include <boost/regex.hpp>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "text_normalization.hpp"

#include "audiofileindex.h"

namespace fs = std::filesystem;

namespace afi {

namespace {

struct RegexPattern {
  Shared::VAType va;
  boost::regex rgx;
};

RegexPattern make(Shared::VAType va, const char* pattern) {
  return {va,
          boost::regex(pattern, boost::regex::icase | boost::regex::optimize)};
}

// Remove the suffix from a string(_view).
std::string_view get_no_suffix(const std::string_view& s) {
  auto pos = s.find_last_of('.');
  if (pos == std::string_view::npos) {
    return s;
  }
  return s.substr(0, pos);
}
std::string_view get_no_suffix(const std::string& s) {
  return get_no_suffix(std::string_view(s));
}

} // namespace

/* clang-format off */
std::vector<RegexPattern> patterns{
  make( // va - (year - )albumTitle/(disc #- disc name/)## - artist - trackTitle[.flac/.mp3/.m4a]
    Shared::VAType::va,
    "^(.*\\/)?((va(rious artists)?)|(compilation)) - "
    "((?<year>\\d{4}) - )?(?<album>[^/]+)"
    "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
    "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"
  ),
  make( // soundtrack - (year - )albumTitle/(disc #- disc name/)## - artist - trackTitle
    Shared::VAType::ost,
    "^(.*\\/)?((ost)|(soundtrack)) - "
    "((?<year>\\d{4}) - )?(?<album>[^/]+)"
    "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
    "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"
  ),
  make( // artist - year - album/(disc #- disc name/)## - trackTitle
    Shared::VAType::none,
    "^(.*\\/)?(?<artist>[^/]+) - (?<year>\\d{4}) - (?<album>[^/]+)"
    "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
    "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"
  ),
  make( // va/(year - )albumTitle/(CD # name/)## - artist - trackTitle
    Shared::VAType::va,
    "^(.*\\/)?((va(rious artists)?)|(compilation))"
    "\\/((?<year>\\d{4}) - )?(?<album>[^/]+)"
    "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
    "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"
  ),
  make( // ost/(year - )albumTitle/(CD # name/)## - artist - trackTitle
    Shared::VAType::ost,
    "^(.*\\/)?((ost)|(soundtrack))"
    "\\/((?<year>\\d{4}) - )?(?<album>[^/]+)"
    "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
    "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"
  ),
  make( // artist/year - albumTitle/CD # name/## - trackTitle
    Shared::VAType::none,
    "^(.*\\/)?(?<artist>[^/]+)\\/(?<year>\\d{4}) - (?<album>[^/]+)"
    "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
    "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"
  ),
  make( // artist/year - albumTitle/## - trackTitle
    Shared::VAType::none,
    "^(.*\\/)?(?<artist>[^/]+)\\/(?<year>\\d{4}) - (?<album>[^/]+)"
    "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"
  ),
  make( // artist - album/CD # name/## - trackTitle
    Shared::VAType::none,
    "^(.*\\/)?(?<artist>[^/]+) - (?<album>[^/]+)"
    "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
    "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"
  ),
  make( // artist/albumTitle/CD # name/## - trackTitle
    Shared::VAType::none,
    "^(.*\\/)?(?<artist>[^/]+)\\/(?<album>[^/]+)"
    "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
    "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"
  ),
  make( // artist - album/## - trackTitle
    Shared::VAType::none,
    "^(.*\\/)?(?<artist>[^/]+) - (?<album>[^/]+)"
    "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"
  ),
  make( // artist/albumTitle/## - trackTitle
    Shared::VAType::none,
    "^(.*\\/)?(?<artist>[^/]+)\\/(?<album>[^/]+)"
    "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"
  )
};
/* clang-format on */

// Metadata stuff:

#pragma region Private interfaces

// Get the metadata for a song from the relative path. (underlying
// implementation for the public interface of "fs::path" or SongKey).
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_rel(
    const std::string& relPath) const {
  if (relPath.length() == 0) {
    return std::nullopt; // NYI
  } else {
    return std::nullopt; // NYI
  }
}

// Get the metadata for a song from the file path only.i
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_from_path_rel(const std::string& relPath) const {
  std::string noSuffix{get_no_suffix(relPath)};
  for (const RegexPattern& pattern : patterns) {
    // Match the pattern against the relPath.
    // If it matches, extract the metadata and return it.
    boost::smatch match;
    if (!boost::regex_match(noSuffix, match, pattern.rgx)) {
      continue;
    }
    // Find
    Shared::FullMetadata metadata;
    metadata.originalPath = relPath;
    // Extract the metadata based on the pattern.
    // NYI!
    return metadata; // Return the extracted metadata.
  }
  return std::nullopt; // No match found.
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_from_file_rel(const std::string& relPath) const {
  if (relPath.length() == 0) {
    return std::nullopt; // NYI
  } else {
    return std::nullopt; // NYI
  }
}

#pragma endregion Private interfaces

#pragma region Public interface

#pragma region Metadata Overrides& caching:
// Replace the local metadata cache with the new metadata.
void audio_file_index::update_metadata(
    const Shared::SongKey& songKey, const Shared::SimpleMetadata& newMetadata) {
  if (songKey.empty() || newMetadata.title.empty()) {
    return; // NYI
  }
  return; // NYI
}

// Replace the local metadata cache with the new metadata.
void audio_file_index::update_metadata(
    const Shared::SongKey& songKey, const Shared::FullMetadata& newMetadata) {
  if (songKey.empty() || newMetadata.title.empty()) {
    return; // NYI
  }
  return; // NYI
}

// Clear the local metadata *cache* (but maintain any overrides).
void audio_file_index::clear_metadata_cache(const Shared::SongKey& songKey) {
  // NYI
  if (songKey.empty()) {
    return;
  }
}

// Clear the local metadata *cache* (but maintain any overrides).
void audio_file_index::clear_metadata_cache() {
  // NYI
}

// Clear the local metadata cache AND overrides.
void audio_file_index::reset_all_metadata(const Shared::SongKey& songKey) {
  // NYI
  if (songKey.empty()) {
    return;
  }
}

// Clear the local metadata cache AND overrides.
void audio_file_index::reset_all_metadata() {
  // NYI
}
#pragma endregion Metadata Overrides& caching

#pragma region Reading metadata from files / paths / cache

// Get the metadata for a song, either from the index, from the file path, or
// from the file metadata itself (in that order of preference).
std::optional<Shared::FullMetadata> audio_file_index::get_metadata(
    const fs::path& filePath) const {
  if (filePath.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song, either from the index, from the file path, or
// from the file metadata itself (in that order of preference).
std::optional<Shared::FullMetadata> audio_file_index::get_metadata(
    const Shared::SongKey& sk) const {
  if (sk.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_from_path(
    const fs::path& filePath) const {
  if (filePath.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_from_path(
    const Shared::SongKey& sk) const {
  if (sk.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_from_file(
    const fs::path& filePath) const {
  if (filePath.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_from_file(
    const Shared::SongKey& sk) const {
  if (sk.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

#pragma endregion Reading metadata from files / paths / cache

#pragma endregion Public interface

} // namespace afi