#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "config.h"
#include "files.h"

namespace config {

// Returns the path to the configuration directory for the application.
std::filesystem::path getPath() {
// This is fugly, most hopefully it's the only truly fugly thing here.
#ifdef _WIN32
  return std::filesystem::path(getenv("LOCALAPPDATA")) / files::GetAppName();
#elif __APPLE__
  return std::filesystem::path(getenv("HOME")) / "Library" /
         "Application Support" / files::GetAppName();
#elif __linux__
  return std::filesystem::path(getenv("HOME")) / ".config" /
         files::GetAppName();
#else
  static_assert(false, "Unsupported platform");
#endif
}

std::unordered_map<std::string, std::string> cache;

// TODO: Make this stuff write to disk (and make it thread-safe :O)
bool writeToStorage(std::string_view key, std::string_view value) {
  cache[std::string{key}] = value;
  return true;
}

std::optional<std::string> readFromStorage(std::string_view key) {
  auto it = cache.find(std::string{key});
  if (it != cache.end()) {
    return it->second;
  }
  return std::nullopt;
}

bool removeFromStorage(std::string_view key) {
  return !!cache.erase(std::string{key});
}

bool flushStorage() {
  cache.clear();
  return true;
}

} // namespace config