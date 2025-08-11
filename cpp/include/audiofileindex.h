#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CommonTypes.hpp"

// Stuff for the afi implemention only?

namespace afi {

using MkMatch =
    std::make_tuple<std::uint8_t, std::vector<Shared::MetadataElement>>;

std::vector<Shared::AudioFileRegexPattern> patterns {
  // va - (year - )albumTitle/(disc #- disc name/)## - artist - track title.flac
  {
    Shared::VAType::va, Shared::MetadataType::Simple,
        {
            MkMatch(4,
                    {Shared::MetadataElement::year,
                     Shared::MetadataElement::album,
                     Shared::MetadataElement::track,
                     Shared::MetadataElement::artist,
                     Shared::MetadataElement::title}),
        },
        "/^(?:.*\/)?(?:(?:va(?:rious artists)?)|(?:compilation)) -"
        //   year       album                      diskNum
        "(?:(\d{4}) - )?([^/]+)(?:\/(?:cd|dis[ck]) *(\d+)"
        //       diskName          trk       artist     title
        "(?:-? +([^ /][^/]+))?)?\/(\d+)[-. ]+([^/]+) - ([^/]+)$"
  }
} // namespace afi

class audio_file_index {
  using path_handler = std::function<void(const std::filesystem::path&)>;

  // Locale and cross-platform file name stuff is completely infuriating:
  std::locale fs_locale;

  // The hash of the index, used to identify it.
  std::size_t hash;
  // The location of the index, where the audio files are stored.
  std::filesystem::path loc;
  // The prefix for the song keys, based on the index hash.
  std::string key_prefix;
  // The index file path, if it exists.
  std::optional<std::filesystem::path> index_file_path;

  // Lookup from canonical, proximate paths to song keys.
  std::unordered_map<std::string, Shared::SongKey> file_to_key;
  // Lookup from song keys to canonical, proximate paths.
  std::unordered_map<Shared::SongKey, std::string> key_to_file;

  // The last time the storage was scanned for files.
  std::chrono::time_point<std::chrono::system_clock> last_scan;
  // Items to ignore in the index (that would otherwise be included).
  std::set<Shared::IgnoreItemPair> ignore_items;

  // This indicates if the file extension of the path is to be tracked.
  bool belongs_here(const std::filesystem::path& path) const;

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

  // Get the metadata for a song from the relative path. (underlying
  // implementation for the public interface of "fs::path" or SongKey).
  std::optional<Shared::FullMetadata> get_metadata_rel(
      const std::string& relPath) const;
  // Get the metadata for a song from the file path only.i
  std::optional<Shared::FullMetadata> get_metadata_from_path_rel(
      const std::string& relPath) const;
  // Get the metadata for a song from the file's metadata only.
  std::optional<Shared::FullMetadata> get_metadata_from_file_rel(
      const std::string& relPath) const;
  // Read the index file from disk and populate the in-memory structures.
  // Returns true if the index file was successfully read, false otherwise.
  bool read_index_file();
  // Write the in-memory structures to the index file on disk.
  void write_index_file() const;

 public:
  // Constructs an audio file index at the given location.
  // If the hash is not provided, it will be computed based on the location.
  // If an index already exists, it will be read from disk, and the source will
  // *NOT* be rescanned.
  audio_file_index(const std::filesystem::path& loc, std::size_t hash = 0)
      : audio_file_index(loc, false, hash) {}
  // Constructs an audio file index at the given location.
  // Update the index, even if it already exists, if update_index is true.
  audio_file_index(const std::filesystem::path& loc,
                   bool update_index,
                   std::size_t hash = 0);

  // No AFI copying!
  audio_file_index(const audio_file_index&) = delete;
  audio_file_index& operator=(const audio_file_index&) = delete;
  audio_file_index(audio_file_index&&) = default;
  audio_file_index& operator=(audio_file_index&&) = default;

  // Get the hash of the index.
  std::size_t get_hash() const {
    return hash;
  }

  // Get the location of the index.
  const std::filesystem::path& get_location() const {
    return loc;
  }

  // The last timue the index was scanned for files.
  std::chrono::time_point<std::chrono::system_clock> get_last_scan_time()
      const {
    return last_scan;
  }

  // Get the song key fora  path (if it belongs to the index).
  std::optional<Shared::SongKey> get_song_key(
      const std::filesystem::path& songPath) const;

  // Just run a function for each audio file in the index.
  void foreach_audio_file(path_handler fn) const;

  // Update the index with any changes, invoking the provided handlers.
  void rescan_files(path_handler addAudioFile, path_handler delAudioFile);

  // Ignore items 'stuff' (NYI):
  void add_ignore_item(Shared::IgnoreItemType which, const std::string& value);
  bool remove_ignore_item(Shared::IgnoreItemType which,
                          const std::string& value);
  std::set<Shared::IgnoreItemPair> get_ignore_items() const;

  // Metadata stuff (NYI):
  // Get the metadata for a song, either from the index, from the file path, or
  // from the file metadata itself (in that order of preference).
  std::optional<Shared::FullMetadata> get_metadata(
      const std::filesystem::path& filePath) const;
  // Get the metadata for a song, either from the index, from the file path, or
  // from the file metadata itself (in that order of preference).
  std::optional<Shared::FullMetadata> get_metadata(
      const Shared::SongKey& sk) const;
  // Get the metadata for a song from the file path only.
  std::optional<Shared::FullMetadata> get_metadata_from_path(
      const std::filesystem::path& filePath) const;
  // Get the metadata for a song from the file path only.
  std::optional<Shared::FullMetadata> get_metadata_from_path(
      const Shared::SongKey& sk) const;
  // Get the metadata for a song from the file's metadata only.
  std::optional<Shared::FullMetadata> get_metadata_from_file(
      const std::filesystem::path& filePath) const;
  // Get the metadata for a song from the file's metadata only.
  std::optional<Shared::FullMetadata> get_metadata_from_file(
      const Shared::SongKey& sk) const;

  // Replace the local metadata cache with the new metadata.
  void update_metadata(const Shared::SongKey& songKey,
                       const Shared::SimpleMetadata& newMetadata);
  // Replace the local metadata cache with the new metadata.
  void update_metadata(const Shared::SongKey& songKey,
                       const Shared::FullMetadata& newMetadata);
  // Clear the local metadata *cache* for a single song (but maintain any
  // overrides).
  void clear_metadata_cache(const Shared::SongKey& songKey);
  // Clear the local metadata cache for all songs.
  void clear_metadata_cache();
  // Clear the local metadata cache AND overrides for a single song.
  void reset_all_metadata(const Shared::SongKey& songKey);
  // Clear the local metadata cache AND overrides.
  void reset_all_metadata();

  // Images for songs & the like (NYI):
  void set_image(const Shared::SongKey& songKey,
                 const std::vector<std::uint8_t>& buf);
  bool clear_image(const Shared::SongKey& songKey);
  std::optional<std::vector<std::uint8_t>> get_image(
      const Shared::SongKey& songKey, bool preferInternal = false) const;
  void clear_local_image_cache();
};

} // namespace afi