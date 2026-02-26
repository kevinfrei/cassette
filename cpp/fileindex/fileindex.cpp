#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "file_tools.hpp"
#include "text_tools.hpp"

#include "fileindex.hpp"

namespace fs = std::filesystem;

file_index::file_index(const fs::path& _loc, bool update_index)
    : loc(std::filesystem::canonical(_loc)),
      last_scan(std::chrono::system_clock::time_point::min()) {

  /*
  if (hash == 0) {
    // Compute hash based on the location, if not provided.
    std::hash<std::string> hasher;
    hash = hasher(loc.generic_string());
  }
  std::ostringstream oss;
  std::uint64_t b64 = base64_string_as_int(hash % 0x7fffffff);
  oss << "S" << reinterpret_cast<const char*>(&b64) << ".";
  key_prefix = oss.str();
  */

  // If there's already an index, read it first, then run a rescan.
  // Maybe rescan on a background thread?
  if (read_index_file() && !update_index) {
    return;
  }
  std::vector<fs::path> newFiles;
  std::vector<fs::path> removedFiles;

  // Read the entire contents of the location into an internal map:
  auto empty = [](const fs::path&) {};
  rescan_files(empty, empty);
}

std::unordered_set<std::string> suffixes = {
    ".mp3", ".flac", ".wav", ".m4a", ".aac", ".wma", ".png", ".jpg"};

bool file_index::belongs_here(const fs::path& path) const {
  // Check if the path has a valid suffix, and is located within the location
  // of the index. We use proximate here to get a path relative to the index
  // location,
  auto prox = fs::proximate(path, loc);
  files::lowercase_extension(prox);
  return prox.is_relative() &&
         suffixes.find(prox.extension().string()) != suffixes.end();
}

// Adds a new file to the index (if it doesn't already exist). Don't save
// anything, just update the in-memory structures.
bool file_index::add_new_file(const fs::path& path) {
  if (!belongs_here(path)) {
    return false;
  }
  std::string relPath = get_relative_path(path);
  if (!file_to_key.contains(relPath)) {
    FileKey key = make_file_key(relPath);
    file_to_key[relPath] = key;
    key_to_file[key] = relPath;
    return true;
  }
  return false;
}

// Removes an existing file from the index (if it exists). Don't save
// anything, just update the in-memory structures.
bool file_index::remove_file(const fs::path& path) {
  std::string relPath = get_relative_path(path);
  if (file_to_key.contains(relPath)) {
    FileKey key = file_to_key[relPath];
    file_to_key.erase(relPath);
    key_to_file.erase(key);
    return true;
  }
  return false;
}

// Get a normalized relative path from the root of the index.
std::string file_index::get_relative_path(const fs::path& path) const {
  auto prox = fs::proximate(path, loc);
  // Normalize to UTF-8 NFC:
  auto str = prox.generic_u8string();
  // if (str.find("onnor") != std::string::npos) {
  //   std::cout << "This one's weird: " << str << "\n";
  //   std::cout << "Normalized: " << txtnorm::normalize_utf8_or_latin(str)
  //             << "\n";
  // }
  return text::normalize_utf8_or_latin(str);
}

bool file_index::read_index_file() {
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
  files::foreach_line_in_file(indexFile, [&](const std::string& line) {
    if (!line.empty()) {
      // The text file is UTF-8, so we can safely reinterpret it as char8_t.
      const std::u8string_view sv =
          reinterpret_cast<const char8_t*>(line.c_str());
      add_new_file(loc / sv);
    }
  });
  return true;
}

void file_index::write_index_file() const {
  if (!fs::exists(loc)) {
    std::cerr << "Index location does not exist: " << loc << "\n";
    return;
  }
  fs::path indexDir = loc / ".afi";
  if (!fs::exists(indexDir)) {
    fs::create_directory(indexDir);
  }
  fs::path indexFile = indexDir / "index.txt";
  // Binary so we don't get any line-ending issues on different platforms.
  std::ofstream ofs(indexFile, std::ios::out | std::ios::binary);
  if (!ofs.is_open()) {
    std::cerr << "Failed to open index file for writing: " << indexFile << "\n";
    return;
  }
  std::set<std::string> sortedFiles;
  // Sort the keys to ensure consistent order in the index file.
  // TODO: Maybe in the future, this is time-ordered?
  for (const auto& [relPath, key] : file_to_key) {
    sortedFiles.insert(relPath);
  }
  for (const auto& relPath : sortedFiles) {
    ofs << relPath << "\n";
  }
  ofs.close();
}

void file_index::rescan_files(path_handler add_file, path_handler del_file) {
  // For simplicity, we will just scan the directory and call addFile
  // for each file found. In a real implementation, we would compare
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
  for (const auto& [relPath, key] : file_to_key) {
    existingFiles.insert(relPath);
  }
  std::set<std::string> newAdds;
  std::set<std::string> newDels;
  for (auto it = fs::recursive_directory_iterator(loc);
       it != fs::recursive_directory_iterator();
       ++it) {
    const fs::path& thePath = it->path();
    if (it->is_directory()) {
      // Skip hidden directories (like .git, .afi, etc...)
      if (thePath.filename().string().starts_with('.')) {
        it.disable_recursion_pending();
      }
    } else if (belongs_here(thePath)) {
      const auto relativePath = get_relative_path(thePath);
      existingFiles.erase(relativePath);
      if (file_to_key.find(relativePath) != file_to_key.end()) {
        // File already indexed, skip it.
        continue;
      }
      // Here we would check if the file is a file based on extension
      // or content. For simplicity, we assume all files are files.
      add_file(relativePath);
      newAdds.insert(relativePath);
    }
  }
  for (const auto& relPath : existingFiles) {
    // These files were not found in the current scan, so they are removed.
    del_file(relPath);
    newDels.insert(relPath);
  }
  // Add/Remove files to/from the index.
  for (const auto& relPath : newAdds) {
    add_new_file(loc / relPath);
  }
  for (const auto& relPath : newDels) {
    remove_file(loc / relPath);
  }
  // Write the index file to disk after the scan is complete.
  write_index_file();
}

file_index::FileKey file_index::make_file_key(
    const std::string& relPath) const {
  // Create a file key based on the relative path.
  // This is a placeholder implementation; actual implementation may vary.
  // TODO: Check for collisions
  return std::hash<std::string>{}(relPath) % 0x7fffffff;
  /*
  std::ostringstream oss;
  std::uint64_t b64 =
      base64_string_as_int(std::hash<std::string>{}(relPath) % 0x7fffffff);
  oss << key_prefix << reinterpret_cast<const char*>(&b64);
  return FileKey{oss.str()};
  */
}

void file_index::foreach_file(path_handler fn) const {
  for (const auto& [relPath, key] : file_to_key) {
    fn(loc / relPath);
  }
}
