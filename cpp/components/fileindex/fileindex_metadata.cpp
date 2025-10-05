#include <filesystem>
#include <optional>
#include <string>

#include <boost/regex.hpp>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "fileindex.hpp"
#include "metadata.hpp"

namespace fs = std::filesystem;

// Metadata stuff:

#pragma region Public interface

#pragma region Metadata Overrides& caching:
// Replace the local metadata cache with the new metadata.
void file_index::update_metadata(const Shared::SongKey& songKey,
                                 const Shared::SimpleMetadata& newMetadata) {
  if (songKey.empty() || newMetadata.title.empty()) {
    return;
  }
  // Untested:
  auto path = this->key_to_file.find(songKey);
  if (path == this->key_to_file.end()) {
    return;
  }
  metadata_cache.write_partial(
      path->second,
      Shared::FullMetadata{
          .title = newMetadata.title,
          .artist = {newMetadata.artist},
          .album = newMetadata.album,
          .year = static_cast<std::int16_t>(std::stoi(newMetadata.year)),
          .track = static_cast<std::int16_t>(std::stoi(newMetadata.track)),
          .diskName = newMetadata.discName,
          .disk = static_cast<std::int16_t>(std::stoi(newMetadata.discNum)),
          .vaType = newMetadata.compilation == Shared::VAType::va
                        ? Shared::VAType::va
                        : Shared::VAType::none});
  return;
}

// Replace the local metadata cache with the new metadata.
void file_index::update_metadata(const Shared::SongKey& songKey,
                                 const Shared::FullMetadata& newMetadata) {
  if (songKey.empty() || newMetadata.title.empty()) {
    return; // NYI
  }
  return; // NYI
}

// Clear the local metadata *cache* (but maintain any overrides).
void file_index::clear_metadata_cache(const Shared::SongKey& songKey) {
  // NYI
  if (songKey.empty()) {
    return;
  }
}

// Clear the local metadata *cache* (but maintain any overrides).
void file_index::clear_metadata_cache() {
  // NYI
}

// Clear the local metadata cache AND overrides.
void file_index::reset_all_metadata(const Shared::SongKey& songKey) {
  // NYI
  if (songKey.empty()) {
    return;
  }
}

// Clear the local metadata cache AND overrides.
void file_index::reset_all_metadata() {
  // NYI
}
#pragma endregion Metadata Overrides& caching

#pragma region Reading metadata from files / paths / cache

// Get the metadata for a song, either from the index, from the file path, or
// from the file metadata itself (in that order of preference).
std::optional<Shared::FullMetadata> file_index::get_metadata(
    const fs::path& filePath) {
  return metadata_cache.read(get_relative_path(filePath));
}

// Get the metadata for a song, either from the index, from the file path, or
// from the file metadata itself (in that order of preference).
std::optional<Shared::FullMetadata> file_index::get_metadata(
    const Shared::SongKey& sk) {
  auto song = key_to_file.find(sk);
  if (song == key_to_file.end()) {
    return std::nullopt; // NYI
  }
  return metadata_cache.read(song->second);
}

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata> file_index::get_metadata_from_path(
    const fs::path& filePath) {
  return metadata_cache.read(get_relative_path(filePath));
}

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata> file_index::get_metadata_from_path(
    const Shared::SongKey& sk) {
  if (sk.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata> file_index::get_metadata_from_file(
    const fs::path& filePath) {
  if (filePath.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata> file_index::get_metadata_from_file(
    const Shared::SongKey& sk) {
  if (sk.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

#pragma endregion Reading metadata from files / paths / cache

#pragma endregion Public interface
