#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "audiofileindex.h"

namespace fs = std::filesystem;

namespace {
// Base 64: A-Za-z0-9+/
std::uint64_t base64_string_as_int(std::uint32_t val) {
  char buffer[sizeof(std::uint64_t)];
  int i = 0;
  for (; i < 6 && val; i++) {
    buffer[i] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"
            [val & 0x3f];
    val >>= 6;
  }
  // Reverse the buffer:
  for (int j = 0; j < i / 2; j++) {
    std::swap(buffer[j], buffer[i - j - 1]);
  }
  if (!i) {
    // Special case for zero:
    buffer[i++] = 'A';
  }
  // Null-terminate the string:
  buffer[i] = '\0';
  std::uint64_t result = *reinterpret_cast<std::uint64_t*>(&buffer);
  return result;
}

void foreach_line_in_file(const fs::path& filePath,
                          const std::function<void(const std::string&)>& fn) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filePath << "\n";
    return;
  }
  std::string line;
  while (std::getline(file, line)) {
    fn(line);
  }
}
} // namespace
namespace afi {

audio_file_index::audio_file_index(const fs::path& _loc,
                                   bool update_index,
                                   std::size_t _hash)
    : hash(_hash),
      loc(std::filesystem::canonical(_loc)),
      last_scan(std::chrono::system_clock::time_point::min()) {
  if (hash == 0) {
    // Compute hash based on the location, if not provided.
    std::hash<std::string> hasher;
    hash = hasher(loc.generic_string());
  }
  std::ostringstream oss;
  std::uint64_t b64 = base64_string_as_int(hash % 0x7fffffff);
  oss << "S" << reinterpret_cast<const char*>(&b64) << ".";
  key_prefix = oss.str();

  // If there's already an index, read it first, then run a rescan.
  // Maybe rescan on a background thread?
  if (read_index_file() && !update_index) {
    return;
  }
  std::vector<fs::path> newFiles;
  std::vector<fs::path> removedFiles;

  // Read the entire contents of the location into an internal map:
  rescan_files(
      [&](const fs::path& path) {
        newFiles.push_back(path);
        // std::cout << "Found audio file: " << path << "\n";
      },
      [&](const fs::path& path) {
        removedFiles.push_back(path);
        // std::cout << "Removed audio file: " << path << "\n";
      });
  // Now we can update the index with the new files.
  for (const auto& path : newFiles) {
    add_new_file(path);
  }
  // And remove the files that were not found.
  for (const auto& path : removedFiles) {
    remove_file(path);
  }
  // TODO: Write the index file to disk.
}

// Adds a new file to the index (if it doesn't already exist). Don't save
// anything, just update the in-memory structures.
bool audio_file_index::add_new_file(const fs::path& path) {
  std::string relPath = get_relative_path(path);
  if (!file_to_key.contains(relPath)) {
    Shared::SongKey songKey = make_song_key(relPath);
    file_to_key[relPath] = songKey;
    key_to_file[songKey] = relPath;
    return true;
  }
  return false;
}

// Removes an existing file from the index (if it exists). Don't save
// anything, just update the in-memory structures.
bool audio_file_index::remove_file(const fs::path& path) {
  std::string relPath = get_relative_path(path);
  if (file_to_key.contains(relPath)) {
    Shared::SongKey songKey = file_to_key[relPath];
    file_to_key.erase(relPath);
    key_to_file.erase(songKey);
    return true;
  }
  return false;
}

// Get a normalized relative path from the root of the index.
std::string audio_file_index::get_relative_path(const fs::path& path) const {
  auto prox = fs::proximate(path, loc);
  auto pstr = prox.generic_string();
  return pstr;
}

bool audio_file_index::read_index_file() {
  if (!fs::exists(loc)) {
    // std::cerr << "Index location does not exist: " << loc << "\n";
    return false;
  }
  fs::path indexDir = loc / ".afi";
  if (!fs::exists(indexDir) || !fs::is_directory(indexDir)) {
    // std::cerr << "Index directory does not exist: " << indexDir << "\n";
    return false;
  }
  fs::path indexFile = indexDir / "index.txt";
  if (!fs::exists(indexFile) || !fs::is_regular_file(indexFile)) {
    // std::cerr << "Index file does not exist: " << indexFile << "\n";
    return false;
  }

  // For simplicity, we will just read the index file line by line.
  // Each line is expected to be a file path relative to loc.
  foreach_line_in_file(indexFile, [&](const std::string& line) {
    if (!line.empty()) {
      /* fs::path filePath = loc / line;
      if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
       */
      add_new_file(loc / line);
      /*
      } else {
        std::cerr << "Indexed file does not exist: " << filePath << "\n";
      }
      */
    }
  });
  return true;
}

std::unordered_set<std::string> suffixes = {
    ".mp3", ".flac", ".wav", ".m4a", ".aac", ".wma", ".png", ".jpg"};

bool audio_file_index::belongs_here(const fs::path& path) const {
  // Check if the path is within the index location.
  return suffixes.contains(
      fs::proximate(path, loc).extension().generic_string());
}

void audio_file_index::rescan_files(path_handler add_audio_file,
                                    path_handler del_audio_file) {
  // For simplicity, we will just scan the directory and call addAudioFile
  // for each audio file found. In a real implementation, we would compare
  // with a cached list of files to determine which files were added or removed.
  if (!fs::exists(loc) || !fs::is_directory(loc)) {
    std::cerr << "Location does not exist or is not a directory: " << loc
              << "\n";
    return;
  }
  last_scan = std::chrono::system_clock::now();
  std::unordered_set<std::string> existingFiles;
  existingFiles.reserve(file_to_key.size());
  // Populate existingFiles with the currently indexed files.
  for (const auto& [relPath, songKey] : file_to_key) {
    existingFiles.insert(relPath);
  }
  for (const auto& entry : fs::recursive_directory_iterator(loc)) {
    if (belongs_here(entry.path())) {
      const auto relativePath = get_relative_path(entry.path());
      existingFiles.erase(relativePath);
      if (file_to_key.contains(relativePath)) {
        // File already indexed, skip it.
        continue;
      }
      // Here we would check if the file is an audio file based on extension
      // or content. For simplicity, we assume all files are audio files.
      add_audio_file(relativePath);
    }
  }
  for (const auto& relPath : existingFiles) {
    // These files were not found in the current scan, so they are removed.
    del_audio_file(relPath);
  }
}

Shared::SongKey audio_file_index::make_song_key(
    const std::string& relPath) const {
  // Create a song key based on the relative path.
  // This is a placeholder implementation; actual implementation may vary.
  // TODO: Check for collisions
  std::ostringstream oss;
  std::uint64_t b64 =
      base64_string_as_int(std::hash<std::string>{}(relPath) % 0x7fffffff);
  oss << key_prefix << reinterpret_cast<const char*>(&b64);
  return Shared::SongKey{oss.str()};
}

void audio_file_index::foreach_audio_file(path_handler fn) const {
  for (const auto& [relPath, songKey] : file_to_key) {
    fn(loc / relPath);
  }
}

} // namespace afi
