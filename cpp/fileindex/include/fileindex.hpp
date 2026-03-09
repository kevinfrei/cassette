#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>

#include "CommonTypes.hpp"

class file_index {
  using path_handler = std::function<void(const std::filesystem::path&)>;
  using FileKey = uint64_t;

  // The location of the index, where the files are stored.
  std::filesystem::path loc;
  // The index file path, if it exists.
  std::optional<std::filesystem::path> index_file_path;

  // Lookup from canonical, proximate paths to file keys.
  std::unordered_map<std::string, FileKey> file_to_key;
  // Lookup from file keys to canonical, proximate paths.
  std::unordered_map<FileKey, std::string> key_to_file;

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
  // Create a file key for a given relative path.
  FileKey make_file_key(const std::string& relPath) const;

  // Read the index file from disk and populate the in-memory structures.
  // Returns true if the index file was successfully read, false otherwise.
  bool read_index_file();
  // Write the in-memory structures to the index file on disk.
  void write_index_file() const;

 public:
  // Constructs a file index at the given location.
  // Update the index, even if it already exists, if update_index is true.
  file_index(const std::filesystem::path& loc, bool update_index = false);

  // No copying!
  file_index(const file_index&) = delete;
  file_index& operator=(const file_index&) = delete;
  file_index(file_index&&) = default;
  file_index& operator=(file_index&&) = default;

  // Get the location of the index.
  const std::filesystem::path& get_location() const {
    return loc;
  }

  // The last timue the index was scanned for files.
  std::chrono::time_point<std::chrono::system_clock> get_last_scan_time()
      const {
    return last_scan;
  }

  // Get the file key for a path (if it belongs to the index).
  std::optional<FileKey> get_file_key(
      const std::filesystem::path& filePath) const;

  // Just run a function for each file in the index.
  void foreach_file(path_handler fn) const;

  // Update the index with any changes, invoking the provided handlers.
  void rescan_files(path_handler addFile, path_handler delFile);

  // Ignore items 'stuff' (NYI):
  void add_ignore_item(Shared::IgnoreItemType which, const std::string& value);
  bool remove_ignore_item(Shared::IgnoreItemType which,
                          const std::string& value);
  std::set<Shared::IgnoreItemPair> get_ignore_items() const;
};
