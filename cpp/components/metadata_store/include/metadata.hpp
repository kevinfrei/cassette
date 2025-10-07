#if !defined(METADATA_HPP)
#define METADATA_HPP
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

#include "CommonTypes.hpp"

namespace metadata {

class store {

  // Note that "path-based" metadata doesn't exist in either of these maps:

  // cached metadata that was pulled out of a file
  std::unordered_map<Shared::SongKey, Shared::FullMetadata> content_cache;
  // metadata that has been explicitly overriden
  std::unordered_map<Shared::SongKey, Shared::FullMetadata> specific_overrides;

  // Storage paths for the cache files
  std::filesystem::path cache_file;
  std::filesystem::path override_file;

 public:
  store(const std::filesystem::path& dir);
  // Get the metadata for a song from any available source. In order:
  // 1. Explicit override
  // 2. Cached content (from prior file based metadata read)
  // 3. Path-based metadata
  // 4. File-based metadata
  std::optional<Shared::FullMetadata> read(const Shared::SongKey& item);
  // This only reads the metadata that was explicitly overridden.
  std::optional<Shared::FullMetadata> read_override(
      const Shared::SongKey& item);
  // This explicitly reads the metadata from the file itself.
  std::optional<Shared::FullMetadata> read_content(const Shared::SongKey& item);
  // This isn't every cached. It's just the metadata that can be extracted
  // from the path.
  std::optional<Shared::FullMetadata> read_path(const Shared::SongKey& item);
  void write_partial(const Shared::SongKey& songKey,
                     const Shared::SimpleMetadata& newMetadata);
  void write_partial(const Shared::SongKey& item,
                     const Shared::FullMetadata& metadata);
  void write_full(const Shared::SongKey& songKey,
                  const Shared::SimpleMetadata& newMetadata);
  void write_full(const Shared::SongKey& item,
                  const Shared::FullMetadata& metadata);

  // Clear the local metadata *cache* for a single song (but maintain any
  // overrides).
  void clear_metadata_cache(const Shared::SongKey& songKey);
  // Clear the local metadata cache for all songs.
  void clear_metadata_cache();
  // Clear the local metadata cache AND overrides for a single song.
  void reset_all_metadata(const Shared::SongKey& songKey);
  // Clear the local metadata cache AND overrides.
  void reset_all_metadata();
  // Clear all overrides.
  void clear_metadata_override();
  // Clear a specific override.
  void clear_metadata_override(const Shared::SongKey& item);
  // Clear everything: cache and overrides.
  void clear();
  // Clear everything for a specific item: cache and overrides.
  void clear(const Shared::SongKey& item);

  // Images for songs & the like (NYI):
  void set_image(const Shared::SongKey& songKey,
                 const std::vector<std::uint8_t>& buf);
  bool clear_image(const Shared::SongKey& songKey);
  std::optional<std::vector<std::uint8_t>> get_image(
      const Shared::SongKey& songKey, bool preferInternal = false) const;
  void clear_local_image_cache();
};

} // namespace metadata

#endif // METADATA_HPP