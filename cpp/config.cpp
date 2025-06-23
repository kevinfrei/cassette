#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "config.h"
#include "files.h"

namespace config {

// This is fugly, most hopefully it's the only truly fugly thing here.
#if defined(_WIN32)
const char* env_var = "LOCALAPPDATA";
std::filesystem::path relative_path = ".";
#elif defined(__APPLE__)
const char* env_var = "HOME";
std::filesystem::path relative_path = "Library/Application Support";
#elif defined(__linux__)
const char* env_var = "HOME";
std::filesystem::path relative_path = "Library/Application Support";
#else
#error Unsupported platform
#endif

// Returns the path to the configuration directory for the application.
std::filesystem::path get_path() {
  return std::filesystem::path(getenv(env_var)) / relative_path /
         files::get_app_name();
}

std::unordered_map<std::string, std::string> cache;

std::filesystem::path get_persistence_path() {
  std::filesystem::path res = get_path() / "persistence";
  if (std::filesystem::exists(res)) {
    return res;
  }
  if (!std::filesystem::create_directories(res)) {
    std::cerr << "Failed to create persistence directory: " << res.string()
              << std::endl;
  }
  return res;
}

// Stuff to keep the cache & disk storage in sync & thread-safe.
std::shared_mutex the_mutex;
using read_lock = std::shared_lock<std::shared_mutex>;
using write_lock = std::unique_lock<std::shared_mutex>;

bool write_to_storage(std::string_view key, std::string_view value) {
  auto path_to_data = get_persistence_path() / files::file_name_encode(key);
  write_lock lock(the_mutex);
  if (std::filesystem::exists(path_to_data)) {
    if (!std::filesystem::remove(path_to_data)) {
      std::cerr << "Failed to remove existing file: " << path_to_data.string()
                << std::endl;
      return false;
    }
  }
  std::ofstream out(path_to_data);
  if (!out) {
    std::cerr << "Failed to open file for writing: " << path_to_data.string()
              << std::endl;
    return false;
  }
  out << value;
  cache[std::string{key}] = value;
  return true;
}

std::optional<std::string> read_from_storage(std::string_view key) {
  std::string key_str{key};
  // Read from the cache first
  {
    read_lock lock(the_mutex);
    auto it = cache.find(key_str);
    if (it != cache.end()) {
      return it->second;
    }
  }
  // We're going to have to modify the cache, so we'll need a write lock.
  // It's helpful to grab it before we do the disk read, so we don't have to
  // deal with races from the write_to_storage function.
  auto path_to_data = get_persistence_path() / files::file_name_encode(key);
  write_lock write_lock(the_mutex);
  if (!std::filesystem::exists(path_to_data)) {
    return std::nullopt; // Key does not exist
  }
  auto maybe_value = files::read_file(path_to_data);
  if (!maybe_value) {
    return std::nullopt; // Failed to read the file
  }
  // Release the read lock before modifying the cache
  cache[key_str] = *maybe_value; // Cache the value
  return *maybe_value;
}

bool delete_from_storage(std::string_view key) {
  write_lock lock(the_mutex);
  cache.erase(std::string{key});
  auto path_to_data = get_persistence_path() / files::file_name_encode(key);
  if (!std::filesystem::exists(path_to_data)) {
    return false; // Key does not exist
  }
  if (!std::filesystem::remove(path_to_data)) {
    std::cerr << "Failed to remove file: " << path_to_data.string()
              << std::endl;
    return false; // Failed to delete the file
  }
  return true;
}

void flush_storage_cache() {
  write_lock lock(the_mutex);
  cache.clear();
}

void clear_storage() {
  flush_storage_cache();
  // TODO: Do this on disk, yeah?
}

} // namespace config