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
  std::optional<Shared::FullMetadata> read(const std::string& item);
  std::optional<Shared::FullMetadata> read_override(const std ::string& item);
  std::optional<Shared::FullMetadata> read_content(const std::string& item);
  std::optional<Shared::FullMetadata> read_path(const std ::string& item);
  void write_partial(const std::string& item,
                     const Shared::FullMetadata& metadata);
  void write_full(const std::string& item,
                  const Shared::FullMetadata& metadata);
};

} // namespace metadata