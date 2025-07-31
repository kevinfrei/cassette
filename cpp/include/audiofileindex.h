#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>

#include "CommonTypes.hpp"

// Stuff for the afi implemention only?

namespace afi {

class audio_file_index {
  std::size_t hash;
  std::filesystem::path loc;
  std::chrono::time_point<std::chrono::system_clock> last_scan;
  std::set<Shared::IgnoreItemPair> ignore_items;

 public:
  using fspath = std::filesystem::path;
  using path_handler = std::function<void(const fspath&)>;

  audio_file_index(const fspath& loc, std::size_t hash = 0);

  std::size_t getHash() const {
    return hash;
  }

  const fspath& getLocation() const {
    return loc;
  }

  // The simple stuff:
  std::chrono::time_point<std::chrono::system_clock> getLastScanTime() const;
  std::optional<Shared::SongKey> makeSongKey(const fspath& songPath) const;
  void forEachAudioFile(path_handler fn) const;
  void rescanFiles(path_handler addAudioFile, path_handler delAudioFile);

  // Metadata stuff:
  void updateMetadata(const Shared::SimpleMetadata& newMetadata);
  void updateMetadata(const Shared::FullMetadata& newMetadata);
  std::optional<Shared::FullMetadata> getMetadataForSong(
      const fspath& filePath) const;
  void clearMetadataCache();
  void clearLocalMetadataOverrides();

  void setImageForSong(const Shared::SongKey& songKey,
                       const std::vector<std::uint8_t>& buf);
  bool clearImageForSong(const Shared::SongKey& songKey);
  std::optional<std::vector<std::uint8_t>> getImageForSong(
      const Shared::SongKey& songKey, bool preferInternal = false) const;
  void clearLocalImageCache();

  void addIgnoreItem(Shared::IgnoreItemType which, const std::string& value);
  bool removeIgnoreItem(Shared::IgnoreItemType which, const std::string& value);
  std::set<Shared::IgnoreItemPair> getIgnoreItems() const;
};

} // namespace afi