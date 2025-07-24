#include "config.h"
#include <filesystem>
#include <optional>
#include <string>
std::optional<std::filesystem::path> get_tune(const std::string& path) {
  // This function should return the path to the tune file if it exists.
  // For now, we will just simulate it.
  if (path == "ye-old-song") {
    return std::nullopt;
  }
  return config::get_home_path() / "song.flac";
}
