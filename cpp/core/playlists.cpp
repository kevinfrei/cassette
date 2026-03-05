#include <optional>
#include <string>
#include <vector>

#include "config.hpp"
#include "files.hpp"
#include "musicdb.hpp"
#include "text_tools.hpp"

#include "playlists.hpp"

namespace playlist {

namespace {

std::optional<Shared::SongKey> song_key_for_playlist_entry(
    std::string_view song_data) {
  // Get the music database and look up this particular song.
  // musicdb::MusicDatabase& db = musicdb::get_music_database();
  return std::nullopt;
}

std::optional<std::string> playlist_entry_for_song_key(std::string_view key) {
  // For now, let's just return the key as the song data. In the future, we
  // should look up the song data for the given key.
  return std::string(key);
}

} // namespace
// I should add support for both "custom" playlists and M3U playlists.
// For now, let's just handle custom playlists. They should be just files
// in the 'playlists' directory, with the (casified) name as the filename
// and the content being the list arist/album/title[/track] of the song.

std::optional<std::vector<std::string>> get_names() {
  auto path = config::get_playlist_path();
  // Read all the files in the playlist directory and return their names
  // (without the .tun extension)
  std::vector<std::string> names;
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_regular_file()) {
      const auto& file_path = entry.path();
      if (text::iequals(file_path.extension().string(), ".tun")) {
        auto maybe_name = files::file_name_decode(file_path.stem().string());
        if (!maybe_name) {
          CROW_LOG_ERROR << "Failed to decode playlist filename: "
                         << file_path.stem().string();
          continue;
        }
        names.push_back(*maybe_name);
      }
    }
  }
  return names;
}

std::optional<std::vector<Shared::SongKey>> load(std::string_view name) {
  auto path = config::get_playlist_path();
  auto file_path =
      path / files::file_name_encode(name).replace_extension(".tun");
  if (!std::filesystem::exists(file_path)) {
    return std::nullopt;
  }
  // Okay, first line should be "# cassette music playlist v#":
  std::ifstream input_file(file_path);
  if (!input_file.is_open()) {
    CROW_LOG_ERROR << "Failed to open playlist file: " << file_path;
    return std::nullopt;
  }
  std::string header;
  std::getline(input_file, header);
  if (header != "# cassette music playlist v1") {
    CROW_LOG_ERROR << "Invalid playlist file header: " << file_path << " -> "
                   << header;
    return std::nullopt;
  }
  std::vector<Shared::SongKey> keys;
  std::string line;
  while (std::getline(input_file, line)) {
    auto maybe_key = song_key_for_playlist_entry(line);
    if (maybe_key) {
      keys.push_back(*maybe_key);
    } else {
      CROW_LOG_ERROR << "Failed to parse playlist entry: " << line
                     << " in file: " << file_path;
    }
  }
  return keys;
}

void save(std::string_view name, const std::vector<std::string_view>& keys) {
  CROW_LOG_INFO << "Saving playlist " << name << " with keys: ";
  for (const auto& key : keys) {
    CROW_LOG_INFO << "  " << key;
  }
  auto path = config::get_playlist_path();
  auto file_path =
      path / files::file_name_encode(name).replace_extension(".tun");
  std::ofstream output_file(file_path);
  if (!output_file.is_open()) {
    CROW_LOG_ERROR << "Failed to open playlist file for writing: " << file_path;
    return;
  }
  output_file << "# cassette music playlist v1\n";
  for (const auto& key : keys) {
    auto maybe_entry = playlist_entry_for_song_key(key);
    if (maybe_entry) {
      output_file << *maybe_entry << "\n";
    } else {
      CROW_LOG_ERROR << "Failed to generate playlist entry for " << file_path
                     << " with key: " << key;
    }
  }
}

} // namespace playlist