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

// Metadata stuff:

#pragma region Private interfaces

// Get the metadata for a song from the relative path. (underlying
// implementation for the public interface of "fs::path" or SongKey).
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_for_song_rel(
    const std::string& relPath) const {
  if (relPath.length() == 0) {
    return std::nullopt; // NYI
  } else {
    return std::nullopt; // NYI
  }
}

// Get the metadata for a song from the file path only.i
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_for_song_from_path_rel(
    const std::string& relPath) const {
  if (relPath.length() == 0) {
    return std::nullopt; // NYI
  } else {
    return std::nullopt; // NYI
  }
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_for_song_from_file_rel(
    const std::string& relPath) const {
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
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_for_song(
    const fs::path& filePath) const {
  if (filePath.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song, either from the index, from the file path, or
// from the file metadata itself (in that order of preference).
std::optional<Shared::FullMetadata> audio_file_index::get_metadata_for_song(
    const Shared::SongKey& sk) const {
  if (sk.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_for_song_from_path(
    const fs::path& filePath) const {
  if (filePath.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file path only.
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_for_song_from_path(
    const Shared::SongKey& sk) const {
  if (sk.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_for_song_from_file(
    const fs::path& filePath) const {
  if (filePath.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

// Get the metadata for a song from the file's metadata only.
std::optional<Shared::FullMetadata>
audio_file_index::get_metadata_for_song_from_file(
    const Shared::SongKey& sk) const {
  if (sk.empty()) {
    return std::nullopt; // NYI
  }
  return std::nullopt; // NYI
}

#pragma endregion Reading metadata from files / paths / cache

#pragma endregion Public interface

} // namespace afi