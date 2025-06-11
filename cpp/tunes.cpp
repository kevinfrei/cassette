#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

std::optional<std::filesystem::path> get_tune(const std::string& path) {
  // This function should return the path to the tune file if it exists.
  // For now, we will just simulate it.
  if (path == "example.mp3") {
    return std::filesystem::path{"/home/freik/music/example.mp3"};
  }
  return std::nullopt;
}
