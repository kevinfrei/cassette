#include <filesystem>
#include <optional>
#include <shared_mutex>
#include <string>

#include <crow/json.h>

#include "config.hpp"

namespace fs = std::filesystem;

namespace tunes {

std::optional<std::filesystem::path> get_tune(const std::string& song_key) {
  // This function should return the path to the tune file if it exists.
  // For now, we will just simulate it.
  if (song_key.empty()) {
    return std::nullopt;
  }
  return config::get_home_path() / "song.mp3";
}

} // namespace tunes
