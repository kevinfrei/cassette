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
  std::unordered_map<std::string, Shared::FullMetadata> content_cache;
  // metadata that has been explicitly overriden
  std::unordered_map<std::string, Shared::FullMetadata> specific_overrides;

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
  std::optional<Shared::FullMetadata> read(const std::filesystem::path& item);
  // This only reads the metadata that was explicitly overridden.
  std::optional<Shared::FullMetadata> read_override(
      const std::filesystem::path& item);
  // This explicitly reads the metadata from the file itself.
  std::optional<Shared::FullMetadata> read_content(
      const std::filesystem::path& item);
  // This isn't every cached. It's just the metadata that can be extracted
  // from the path.
  std::optional<Shared::FullMetadata> read_path(
      const std::filesystem::path& item);
  void write_partial(const std::filesystem::path& item,
                     const Shared::SimpleMetadata& newMetadata);
  void write_partial(const std::filesystem::path& item,
                     const Shared::FullMetadata& metadata);
  void write_full(const std::filesystem::path& item,
                  const Shared::SimpleMetadata& newMetadata);
  void write_full(const std::filesystem::path& item,
                  const Shared::FullMetadata& metadata);

  // Clear the local metadata *cache* for a single song (but maintain any
  // overrides).
  void clear_metadata_cache(const std::filesystem::path& item);
  // Clear the local metadata cache for all songs.
  void clear_metadata_cache();
  // Clear the local metadata cache AND overrides for a single song.
  void reset_all_metadata(const std::filesystem::path& item);
  // Clear the local metadata cache AND overrides.
  void reset_all_metadata();
  // Clear all overrides.
  void clear_metadata_override();
  // Clear a specific override.
  void clear_metadata_override(const std::filesystem::path& item);
  // Clear everything: cache and overrides.
  void clear();
  // Clear everything for a specific item: cache and overrides.
  void clear(const std::filesystem::path& item);

  // Images for songs & the like (NYI):
  void set_image(const std::filesystem::path& item,
                 const std::vector<std::uint8_t>& buf);
  bool clear_image(const std::filesystem::path& item);
  std::optional<std::vector<std::uint8_t>> get_image(
      const std::filesystem::path& item, bool preferInternal = false) const;
  void clear_local_image_cache();
};

} // namespace metadata
