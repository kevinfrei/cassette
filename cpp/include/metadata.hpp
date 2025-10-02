#include <filesystem>
#include <optional>
#include <unordered_map>

#include "CommonTypes.hpp"

namespace metadata {

class cache {

  // Note that "path-based" metadata doesn't exist in either of these maps:

  // cached metadata that was pulled out of a file
  std::unordered_map<std::string, Shared::FullMetadata> content_cache;
  // metadata that has been explicitly overriden
  std::unordered_map<std::string, Shared::FullMetadata> specific_overrides;

 public:
  cache(const std::filesystem::path& dir);
  // Get the metadata for a song from any available source. In order:
  // 1. Explicit override
  // 2. Cached content (from prior file based metadata read)
  // 3. Path-based metadata
  // 4. File-based metadata
  std::optional<Shared::FullMetadata> read(const std::string& item);
  // This only reads the metadata that was explicitly overridden.
  std::optional<Shared::FullMetadata> read_override(const std::string& item);
  // This explicitly reads the metadata from the file itself.
  std::optional<Shared::FullMetadata> read_content(const std::string& item);
  // This isn't every cached. It's just the metadata that can be extracted
  // from the path.
  std::optional<Shared::FullMetadata> read_path(const std::string& item);
  void write_partial(const std::string& item,
                     const Shared::FullMetadata& metadata);
  void write_full(const std::string& item,
                  const Shared::FullMetadata& metadata);

  void clear();
  void clear(const std::string& item);
  void clear_metadata_cache();
  void clear_metadata_cache(const std::string& item);
  void clear_metadata_override();
  void clear_metadata_override(const std::string& item);
};

} // namespace metadata