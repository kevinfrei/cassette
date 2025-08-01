#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CommonTypes.hpp"

// Stuff for the afi implemention only?

namespace afi {

class audio_file_index {
  using path_handler = std::function<void(const std::filesystem::path&)>;

  std::size_t hash;
  std::filesystem::path loc;
  std::string key_prefix;

  std::unordered_map<std::string, Shared::SongKey> file_to_key;
  std::unordered_map<Shared::SongKey, std::string> key_to_file;

  std::chrono::time_point<std::chrono::system_clock> last_scan;
  std::set<Shared::IgnoreItemPair> ignore_items;

  // Adds a new file to the index (if it doesn't already exist). Don't save
  // anything, just update the in-memory structures.
  bool add_new_file(const std::filesystem::path& path);
  // Removes an existing file from the index (if it exists). Don't save
  // anything, just update the in-memory structures.
  bool remove_file(const std::filesystem::path& path);
  // Get a normalized relative path from the root of the index.
  std::string get_relative_path(const std::filesystem::path& path) const;
  // Create a song key for a given relative path.
  Shared::SongKey make_song_key(const std::string& relPath) const;

  void read_index_file();
  void write_index_file() const;

 public:
  audio_file_index(const std::filesystem::path& loc, std::size_t hash = 0);

  std::size_t get_hash() const {
    return hash;
  }

  const std::filesystem::path& get_location() const {
    return loc;
  }

  // The simple stuff:
  std::chrono::time_point<std::chrono::system_clock> get_last_scan_time() const;
  std::optional<Shared::SongKey> get_song_key(
      const std::filesystem::path& songPath) const;
  void foreach_audio_file(path_handler fn) const;
  void rescan_files(path_handler addAudioFile, path_handler delAudioFile);

  // Metadata stuff:
  void update_metadata(const Shared::SimpleMetadata& newMetadata);
  void update_metadata(const Shared::FullMetadata& newMetadata);
  std::optional<Shared::FullMetadata> get_metadata_for_song(
      const std::filesystem::path& filePath) const;
  void clear_metadata_cache();
  void clear_local_metadata_overrides();

  void set_image_for_song(const Shared::SongKey& songKey,
                          const std::vector<std::uint8_t>& buf);
  bool clear_image_for_song(const Shared::SongKey& songKey);
  std::optional<std::vector<std::uint8_t>> get_image_for_song(
      const Shared::SongKey& songKey, bool preferInternal = false) const;
  void clear_local_image_cache();

  void add_ignore_item(Shared::IgnoreItemType which, const std::string& value);
  bool remove_ignore_item(Shared::IgnoreItemType which,
                          const std::string& value);
  std::set<Shared::IgnoreItemPair> get_ignore_items() const;
};

} // namespace afi