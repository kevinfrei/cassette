#include <filesystem>
#include <optional>
#include <shared_mutex>
#include <string>

#include <crow/json.h>

#include "config.hpp"
#include "musicdb.hpp"
#include "tunes.hpp"

namespace fs = std::filesystem;

namespace {
std::shared_mutex get_db_mutex;

#if defined(_WIN32)
const char* user_root = "USERPROFILE";
#elif defined(__APPLE__) || defined(__linux__)
const char* user_root = "HOME";
#else
#error Unsupported platform
#endif

} // namespace

namespace tunes {

std::optional<std::filesystem::path> get_tune(const std::string& path) {
  // This function should return the path to the tune file if it exists.
  // For now, we will just simulate it.
  if (path.empty() || !path.empty()) {
    return config::get_home_path() / "song.m4a";
  }
  return std::nullopt;
}

Shared::MusicDatabase* get_music_db() {
  static Shared::MusicDatabase* music_db = nullptr;
  if (music_db) {
    return music_db; // Already initialized
  }
  std::unique_lock<std::shared_mutex> lock(get_db_mutex);
  if (music_db) {
    // Ah, double-checked locking, you're so weird...
    return music_db;
  }
  // Get the location from the config.
  auto maybe_roots_json = config::read_from_storage(
      Shared::to_string(Shared::StorageId::Locations));
  std::vector<fs::path> roots;
  if (maybe_roots_json.has_value() && !maybe_roots_json->empty()) {
    // TODO: This is wrong for now...
    auto maybe_roots = from_json<std::vector<std::string>>(
        crow::json::load(*maybe_roots_json));
    if (!maybe_roots || maybe_roots->empty()) {
      std::cerr << "Failed to parse music locations from storage." << std::endl;
      return nullptr;
    }
    for (const auto& root : *maybe_roots) {
      roots.push_back(fs::path(root));
    }
  }
  if (roots.empty()) {
    // TODO: Not sure about this stuff
#if defined(__APPLE__) && false
    roots.push_back(fs::path("/Volumes/DDrive$/Audio/Sorted"));
#else
    roots.push_back(fs::path(getenv(user_root)) / "Music");
#endif
  }
  for (const auto& root : roots) {
    if (!fs::exists(root)) {
      std::cerr << "Music directory does not exist: " << root.string()
                << std::endl;
    }
  }
  auto mdb = new musicdb::MusicDatabase();
  for (const auto& root : roots) {
    mdb->addFileLocation(root);
  }
  music_db = new Shared::MusicDatabase(mdb->getDatabase());
  return music_db;
}

void send_music_db(crow::websocket::connection& conn) {
  // Send the music database to the client.
  Shared::MusicDatabase* db = get_music_db();
  if (!db) {
    std::cerr << "Failed to get music database!" << std::endl;
    return;
  }
  std::ostringstream oss;
  oss << static_cast<uint64_t>(Shared::SocketMsg::MusicDBUpdate) << ";"
      << to_json(*db).dump();

  conn.send_text(oss.str());
}

} // namespace tunes
