#include <charconv>
#include <filesystem>
#include <optional>
#include <string>

#include <boost/regex.hpp>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
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

std::vector<RegexPattern> patterns{
    // va - (year - )albumTitle/(disc #- disc name/)## - artist - trackTitle
    make(Shared::VAType::va,
         "^(.*\\/)?((va(rious artists)?)|(compilation)) - "
         "((?<year>\\d{4}) - )?(?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"),
    // ost - (year - )albumTitle/(disc #- disc name/)## - artist - trackTitle
    make(Shared::VAType::ost,
         "^(.*\\/)?((ost)|(soundtrack)) - "
         "((?<year>\\d{4}) - )?(?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"),
    // artist - year - album/(disc #- disc name/)## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+) - (?<year>\\d{4}) - (?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"),
    // va/(year - )albumTitle/(CD # name/)## - artist - trackTitle
    make(Shared::VAType::va,
         "^(.*\\/)?((va(rious artists)?)|(compilation))"
         "\\/((?<year>\\d{4}) - )?(?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"),
    // ost/(year - )albumTitle/(CD # name/)## - artist - trackTitle
    make(Shared::VAType::ost,
         "^(.*\\/)?((ost)|(soundtrack))"
         "\\/((?<year>\\d{4}) - )?(?<album>[^/]+)"
         "(\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?)?"
         "\\/(?<track>\\d+)[-. ]+(?<artist>[^/]+) - (?<title>[^/]+)$"),
    // artist/year - albumTitle/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+)\\/(?<year>\\d{4}) - (?<album>[^/]+)"
         "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
         "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"),
    // artist/year - albumTitle/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+)\\/(?<year>\\d{4}) - (?<album>[^/]+)"
         "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"),
    // artist - album/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+) - (?<album>[^/]+)"
         "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
         "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"),
    // artist/albumTitle/CD # name/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+)\\/(?<album>[^/]+)"
         "\\/(cd|dis[ck]) *(?<discNum>\\d+)(-? +(?<discName>[^ /][^/]+))?"
         "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$"),
    // artist - album/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+) - (?<album>[^/]+)"
         "\\/(?<track>\\d+)[-. ]+(?<title>[^/]+)$"),
    // artist/albumTitle/## - trackTitle
    make(Shared::VAType::none,
         "^(.*\\/)?(?<artist>[^/]+)\\/(?<album>[^/]+)"
         "\\/(?<track>\\d+)[-. ]+ (?<title>[^/]+)$")};

// Metadata stuff:

#pragma region Private interfaces

// Get the metadata for a song from the relative path. (underlying
// implementation for the public interface of "fs::path" or SongKey).
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_rel(
    const std::string& relPath) const {
  // TODO: use a cache, overrides, the file path, then the file metadata.
  return get_metadata_from_path_rel(relPath);
}

// Get the metadata for a song from the file path only.i
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_from_path_rel(const std::string& relPath) const {
  std::string noSuffix{get_no_suffix(relPath)};
  for (const RegexPattern& pattern : patterns) {
    // Match the pattern against the relPath.
    // If it matches, extract the metadata and return it.
    boost::cmatch match;
    if (!boost::regex_match(noSuffix.c_str(), match, pattern.rgx)) {
      continue;
    }
    Shared::FullMetadata metadata;
    metadata.originalPath = relPath;
    // Extract the metadata based on the pattern.
    auto artist = match["artist"];
    if (artist.matched) {
      metadata.artist.push_back(artist.str());
    }
    auto album = match["album"];
    if (album.matched) {
      metadata.album = album.str();
    }
    auto year = match["year"];
    if (year.matched) {
      std::from_chars(year.first, year.second, metadata.year);
    }
    auto track = match["track"];
    if (track.matched) {
      std::from_chars(track.first, track.second, metadata.track);
    }
    auto title = match["title"];
    if (title.matched) {
      metadata.title = title.str();
    }
    auto discNum = match["discNum"];
    if (discNum.matched) {
      std::from_chars(discNum.first, discNum.second, metadata.disk);
    } else {
      metadata.disk = 0; // Default to 0 if not matched.
    }
    auto discName = match["discName"];
    if (discName.matched) {
      metadata.diskName = discName.str();
    }
    metadata.vaType = pattern.va;
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
  return get_metadata_rel(get_relative_path(filePath));
}

// Get the metadata for a song, either from the index, from the file path, or
// from the file metadata itself (in that order of preference).
std::optional<Shared::FullMetadata> audio_file_index::get_metadata(
    const Shared::SongKey& sk) const {
  auto song = key_to_file.find(sk);
  if (song == key_to_file.end()) {
    return std::nullopt; // NYI
  }
  return get_metadata_rel(song->second);
}

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_from_path(
    const fs::path& filePath) const {
  return get_metadata_rel(get_relative_path(filePath));
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