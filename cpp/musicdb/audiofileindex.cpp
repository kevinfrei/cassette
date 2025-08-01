#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "audiofileindex.h"

namespace fs = std::filesystem;

namespace {
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
}
namespace afi {

audio_file_index::audio_file_index(const fs::path& _loc, std::size_t _hash)
    : hash(_hash), loc(std::filesystem::canonical(_loc)) {
  if (hash == 0) {
    // Compute hash based on the location, if not provided.
    std::hash<std::string> hasher;
    hash = hasher(loc.generic_string());
  }

  // If there's already an index, read it first, then run a rescan.
  // Maybe rescan on a background thread?
  read_index_file();
  /*
  std::vector<fs::path> newFiles;
  std::vector<fs::path> removedFiles;

  // Read the entire contents of the location into an internal map:
  rescanFiles(
      [&](const fs::path& path) {
        newFiles.push_back(path);
        // std::cout << "Found audio file: " << path << "\n";
      },
      [&](const fs::path& path) {
        removedFiles.push_back(path);
        // std::cout << "Removed audio file: " << path << "\n";
      });
  */
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
  return fs::proximate(path, loc).generic_string();
}

void audio_file_index::read_index_file() {
  if (!fs::exists(loc)) {
    std::cerr << "Index location does not exist: " << loc << "\n";
    return;
  }
  fs::path indexDir = loc / ".afi";
  if (!fs::exists(indexDir) || !fs::is_directory(indexDir)) {
    std::cerr << "Index directory does not exist: " << indexDir << "\n";
    return;
  }
  fs::path indexFile = indexDir / "index.txt";
  if (!fs::exists(indexFile) || !fs::is_regular_file(indexFile)) {
    std::cerr << "Index file does not exist: " << indexFile << "\n";
    return;
  }

  // For simplicity, we will just read the index file line by line.
  // Each line is expected to be a file path relative to loc.
  foreach_line_in_file(indexFile, [&](const std::string& line) {
    if (!line.empty()) {
      fs::path filePath = loc / line;
      if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
        add_new_file(filePath);
      } else {
        std::cerr << "Indexed file does not exist: " << filePath << "\n";
      }
    }
  });
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

  for (const auto& entry : fs::recursive_directory_iterator(loc)) {
    if (entry.is_regular_file()) {
      const auto& path = entry.path();
      // Here we would check if the file is an audio file based on extension
      // or content. For simplicity, we assume all files are audio files.
      add_audio_file(path);
    }
  }

} // namespace afi
