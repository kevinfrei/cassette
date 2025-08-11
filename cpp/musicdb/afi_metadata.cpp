#include <filesystem>
#include <optional>
#include <string>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "text_normalization.hpp"

#include "audiofileindex.h"

namespace fs = std::filesystem;

namespace afi {

namespace {
std::tuple<std::uint8_t, std::vector<Shared::MetadataElement>> MkMatch(
    std::uint8_t numElements, std::vector<Shared::MetadataElement>&& elements) {
  return std::make_tuple(numElements, std::move(elements));
}
} // namespace
std::vector<Shared::AudioFileRegexPattern> patterns{
    // va - (year - )album/(disc #- disc name/)## - artist - title.flac
    {Shared::VAType::va,
     Shared::MetadataType::Simple,
     {MkMatch(4,
              {Shared::MetadataElement::album,
               Shared::MetadataElement::track,
               Shared::MetadataElement::artist,
               Shared::MetadataElement::title}),
      MkMatch(5,
              {Shared::MetadataElement::year,
               Shared::MetadataElement::album,
               Shared::MetadataElement::track,
               Shared::MetadataElement::artist,
               Shared::MetadataElement::title}),
      MkMatch(6,
              {Shared::MetadataElement::album,
               Shared::MetadataElement::diskNum,
               Shared::MetadataElement::diskName,
               Shared::MetadataElement::track,
               Shared::MetadataElement::artist,
               Shared::MetadataElement::title}),
      MkMatch(7,
              {Shared::MetadataElement::year,
               Shared::MetadataElement::album,
               Shared::MetadataElement::diskNum,
               Shared::MetadataElement::diskName,
               Shared::MetadataElement::track,
               Shared::MetadataElement::artist,
               Shared::MetadataElement::title})},
     "/^(?:.*\\/)?(?:(?:va(?:rious artists)?)|(?:compilation)) - "
     //   year       album                      diskNum
     "(?:(\\d{4}) - )?([^/]+)(?:\\/(?:cd|dis[ck]) *(\\d+)"
     //       diskName          trk       artist     title
     "(?:-? +([^ /][^/]+))?)?\\/(\\d+)[-. ]+([^/]+) - ([^/]+)$"},

    // ost - (year - )album/(disc #- disc name/)## - artist - title.flac
    {Shared::VAType::ost,
     Shared::MetadataType::Simple,
     {MkMatch(4,
              {Shared::MetadataElement::album,
               Shared::MetadataElement::track,
               Shared::MetadataElement::artist,
               Shared::MetadataElement::title}),
      MkMatch(5,
              {Shared::MetadataElement::year,
               Shared::MetadataElement::album,
               Shared::MetadataElement::track,
               Shared::MetadataElement::artist,
               Shared::MetadataElement::title}),
      MkMatch(6,
              {Shared::MetadataElement::album,
               Shared::MetadataElement::diskNum,
               Shared::MetadataElement::diskName,
               Shared::MetadataElement::track,
               Shared::MetadataElement::artist,
               Shared::MetadataElement::title}),
      MkMatch(7,
              {Shared::MetadataElement::year,
               Shared::MetadataElement::album,
               Shared::MetadataElement::diskNum,
               Shared::MetadataElement::diskName,
               Shared::MetadataElement::track,
               Shared::MetadataElement::artist,
               Shared::MetadataElement::title})},
     "^(?:.*\\/)?(?:(?:ost)|(?:soundtrack)) - "
     //   year         album                   diskNum
     "(?:(\\d{4}) - )?([^/]+)(?:\\/(cd|dis[ck]) *(\\d+)"
     //       diskName           trk         artist    title
     "(?:-? +([^ /][^/]+))?)?\\/(\\d+)[-. ]+([^/]+) - ([^/]+)$"},
    // artist - year - album/(disc #- disc name/)## - track title.flac
    {Shared::VAType::none,
     Shared::MetadataType::Simple,
     {MkMatch(5,
              {Shared::MetadataElement::artist,
               Shared::MetadataElement::year,
               Shared::MetadataElement::album,
               Shared::MetadataElement::track,
               Shared::MetadataElement::title}),
      MkMatch(7,
              {Shared::MetadataElement::artist,
               Shared::MetadataElement::year,
               Shared::MetadataElement::album,
               Shared::MetadataElement::diskNum,
               Shared::MetadataElement::diskName,
               Shared::MetadataElement::track,
               Shared::MetadataElement::title})},
     //            artist    year       album
     "/^(?:.*\\/)?([^/]+) - (\\d{4}) - ([^/]+)"
     //                  diskNum    diskName
     "(\\/(cd|dis[ck]) *(\\d+)(-? +([^ /][^/]+))?)?"
     //  track        title
     "\\/(\\d+)[-. ]+([^/]+)$"}};

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
  if (relPath.length() == 0) {
    return std::nullopt; // NYI
  } else {
    return std::nullopt; // NYI
  }
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