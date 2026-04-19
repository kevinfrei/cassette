#include <filesystem>
#include <optional>
#include <shared_mutex>
#include <string>

#include <crow/json.h>
#include <sago/platform_folders.h>

#include "musicdb.hpp"
#include "tunes.hpp"

namespace fs = std::filesystem;

namespace {

std::shared_mutex get_db_mutex;

} // namespace

namespace tunes {

std::optional<fs::path> get_tune(const std::string& song_key) {
  // This function should return the path to the tune file if it exists.
  // For now, we will just simulate it.
  if (song_key.empty()) {
    return std::nullopt;
  }
  auto& mdb = musicdb::get();
  return mdb.get_song_path(song_key);
}

const Shared::MusicDatabase& get_music_db() {
  return musicdb::get().get_flat_database();
}

void send_music_db(crow::websocket::connection& conn) {
  // Send the music database to the client.
  auto& mdb = musicdb::get();
  auto flat = mdb.get_flat_database();
  std::ostringstream oss;
  oss << static_cast<uint64_t>(Shared::SocketMsg::MusicDBUpdate) << ";"
      << to_json(flat).dump();

  conn.send_text(oss.str());
}

} // namespace tunes
