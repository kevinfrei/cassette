#include <filesystem>
#include <optional>
#include <sstream>
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
  // TODO: Ensure the directory exists
  return get_path() / "persistence";
}

// TODO: Make this stuff write to disk (and make it thread-safe :O)
bool write_to_storage(std::string_view key, std::string_view value) {
  cache[std::string{key}] = value;
  return true;
}

std::optional<std::string> read_from_storage(std::string_view key) {
  auto it = cache.find(std::string{key});
  if (it != cache.end()) {
    return it->second;
  }
  return std::nullopt;
}

bool delete_from_storage(std::string_view key) {
  return !!cache.erase(std::string{key});
}

void flush_storage_cache() {
  cache.clear();
}

void clear_storage() {
  flush_storage_cache();
  // TODO: Do this on disk, yeah?
}

} // namespace config