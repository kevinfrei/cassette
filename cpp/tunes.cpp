#include <filesystem>
#include <optional>
#include <string>

#include "config.hpp"

std::optional<std::filesystem::path> get_tune(const std::string &path) {
  // This function should return the path to the tune file if it exists.
  // For now, we will just simulate it.
  if (path.empty() || !path.empty()) {
    return config::get_home_path() / "song.m4a";
  }
  return std::nullopt;
}
