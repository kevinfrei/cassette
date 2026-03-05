#include <cstddef>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "CommonTypes.hpp"
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
  musicdb::MusicDatabase& mdb = musicdb::get();

  size_t end_of_track = song_data.find('\t');
  if (end_of_track == song_data.npos) {
    return std::nullopt;
  }
  size_t end_of_album_title = song_data.find('\t', end_of_track + 1);
  if (end_of_album_title == song_data.npos) {
    return std::nullopt;
  }
  size_t end_of_album_year = song_data.find('\t', end_of_album_title + 1);
  if (end_of_album_year == song_data.npos) {
    return std::nullopt;
  }

  std::string_view track_str = song_data.substr(0, end_of_track);
  std::optional<std::uint16_t> mtrack_num =
      text::to_integer<std::uint16_t>(track_str);
  if (!mtrack_num) {
    return std::nullopt;
  }
  std::uint16_t track_num = *mtrack_num;
  std::string album_title{
      song_data.substr(end_of_track + 1, end_of_album_title)};
  std::string_view album_year_str =
      song_data.substr(end_of_album_title + 1, end_of_album_year);
  std::optional<std::uint16_t> malbum_year =
      text::to_integer<std::uint16_t>(album_year_str);
  std::uint16_t album_year = !malbum_year ? 0 : *malbum_year;
  std::string_view artists_or_va = song_data.substr(end_of_album_year + 1);

  Shared::VAType vaType;
  std::vector<std::string> artists;
  // TODO: This stuff probably belongs in the music db code, since the encoding
  // happens in there.
  if (*artists_or_va.rbegin() == '*') {
    // It's a VA album
    std::optional<Shared::VAType> maybe_va =
        Shared::from_string<Shared::VAType>(
            artists_or_va.substr(0, artists_or_va.length() - 1));
    if (!maybe_va) {
      return std::nullopt;
    }
    vaType = *maybe_va;
  } else {
    vaType = Shared::VAType::none;
    // Split this up using all the "|"
    size_t start = 0, end;
    for (end = artists_or_va.find('|', start + 1); end != artists_or_va.npos;
         end = artists_or_va.find('|', start)) {
      artists.push_back(std::string{artists_or_va.substr(start, end)});
      start = end + 1;
    }
  }
  std::optional<Shared::AlbumKey> alb =
      mdb.get_album(album_title, album_year, artists, vaType);
  if (!alb) {
    return std::nullopt;
  }
  auto malbum = mdb.get_album_from_key(*alb);
  if (!malbum) {
    return std::nullopt;
  }
  auto song = malbum->songs.find(track_num);
  if (song == malbum->songs.end()) {
    return std::nullopt;
  }
  return song->second;
}

std::optional<std::string> playlist_entry_for_song_key(std::string_view key) {
  // For now, let's just return the key as the song data. In the future, we
  // should look up the song data for the given key.
  musicdb::MusicDatabase& mdb = musicdb::get();
  const auto maybe_song = mdb.get_song(Shared::SongKey(key));
  if (!maybe_song)
    return std::nullopt;
  const Shared::SongWithPath& song = *maybe_song;
  const auto album = mdb.get_album_from_key(song.albumId);
  if (!album) {
    CROW_LOG_ERROR << "Invalid album from song ID " << key;
    return std::nullopt;
  }
  auto triple = mdb.make_album_triple(
      album->title, album->year, album->primaryArtists, album->vatype);
  std::ostringstream oss;
  oss << song.track << "\t" << std::get<0>(triple) << "\t"
      << std::get<1>(triple) << "\t" << std::get<2>(triple);
  return oss.str();
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