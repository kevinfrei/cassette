#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <shared_mutex>
#include <sstream>
#include <type_traits>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "musicdb.hpp"

namespace fs = std::filesystem;

#if defined(_WIN32)
const char* user_root = "USERPROFILE";
#elif defined(__APPLE__) || defined(__linux__)
const char* user_root = "HOME";
#else
#error Unsupported platform
#endif

namespace musicdb {

namespace {

std::shared_mutex music_db_mutex;
std::array<std::string, 5> extensions = {
    ".mp3", ".flac", ".m4a", ".jpg", ".png"};
std::array<std::string, 3> audio_ext = {".mp3", ".flac", ".m4a"};
MusicDatabase* mdb = nullptr;

} // namespace

Shared::MusicDatabase* get_music_db() {
  static Shared::MusicDatabase* music_db = nullptr;
  if (music_db) {
    return music_db; // Already initialized
  }
  std::unique_lock<std::shared_mutex> lock(music_db_mutex);
  if (music_db) {
    // Ah, double-checked locking, you're so weird...
    return music_db;
  }
// TODO: Get the location from the config.
#if defined(__APPLE__)
  fs::path root = "/Volumes/DDrive$/Audio/Sorted";
#else
  std::string home = getenv(user_root);
  fs::path root = fs::path(home) / "Music";
#endif
  if (!fs::exists(root)) {
    std::cerr << "Music directory does not exist: " << root.string()
              << std::endl;
    return nullptr;
  }
  mdb = new MusicDatabase();
  mdb->addFileLocation(root);
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

MusicDatabase::~MusicDatabase() {
  if (audioIndex) {
    delete audioIndex;
    audioIndex = nullptr;
  }
  if (metadata_cache) {
    delete metadata_cache;
    metadata_cache = nullptr;
  }
}

bool MusicDatabase::addFileLocation(const std::filesystem::path& root) {
  // NYI
  // For now, this is used to add a single file for testing.
  if (audioIndex) {
    delete audioIndex;
    delete metadata_cache;
  }
  audioIndex = new file_index(root, true);
  metadata_cache = new metadata::store(root);
  audioIndex->foreach_file([this](const fs::path& p) {
    std::string ext = p.extension().string();
    for (const auto& validExt : audio_ext) {
      if (ext == validExt) {
        addSongToDB(p);
        break;
      }
    }
  });
  return true;
}
bool MusicDatabase::removeFileLocation(const std::filesystem::path& str) {
  // NYI
  return false;
}

std::vector<std::string> MusicDatabase::getLocations() const {
  if (audioIndex) {
    return std::vector<std::string>{
        audioIndex->get_location().generic_string()};
  }
  return std::vector<std::string>{};
}

std::string MusicDatabase::normalized_path(const std::filesystem::path& p) {
  return std::filesystem::weakly_canonical(p).generic_string();
}

Shared::ArtistKey MusicDatabase::getOrCreateArtist(
    const std::string& artistName) {
  // TODO: Make this case-insensitive
  auto it = artist_name_to_key.find(artistName);
  if (it != artist_name_to_key.end()) {
    return it->second;
  }

  Shared::ArtistKey newKey = "R" + std::to_string(artist_name_to_key.size());
  artist_name_to_key[artistName] = newKey;
  Shared::Artist artistEntry;
  artistEntry.key = newKey;
  artistEntry.name = artistName;
  artists[newKey] = artistEntry;
  return newKey;
}

Shared::AlbumKey MusicDatabase::getOrCreateAlbum(
    const std::string& title,
    std::int16_t year,
    const std::vector<std::string>& artists,
    Shared::VAType vaType) {
  std::string artistHashValue;
  if (vaType != Shared::VAType::none) {
    // For VA albums, we can't really do a good job of deduping.
    artistHashValue = Shared::to_string(vaType);
    artistHashValue.push_back('*');
  } else {
    // TODO: Make this case-insensitive
    for (const auto& artist : artists) {
      artistHashValue += artist + "|";
    }
  }
  AlbumTriple keyTuple = {title, year, artistHashValue};
  auto it = album_year_artist_to_key.find(keyTuple);
  if (it != album_year_artist_to_key.end()) {
    return it->second;
  }
  Shared::AlbumKey newKey =
      "L" + std::to_string(album_year_artist_to_key.size());
  album_year_artist_to_key[keyTuple] = newKey;
  Shared::Album albumEntry;
  albumEntry.key = newKey;
  albumEntry.year = year;
  albumEntry.title = title;
  albumEntry.vatype = vaType;
  for (const auto& artist : artists) {
    Shared::ArtistKey artistKey = getOrCreateArtist(artist);
    albumEntry.primaryArtists.push_back(artistKey);
    // Now, add this album to the artist's list of albums:
    Shared::Artist& theArtist = this->artists[artistKey];
    theArtist.albums.push_back(newKey);
  }
  albums[newKey] = albumEntry;
  return newKey;
}

void MusicDatabase::addSongToDB(const fs::path& song) {
  // First, get the metadata for the song
  auto md = metadata_cache->read(song);
  if (!md) {
    std::cerr << "Failed to get metadata for song: " << song.string()
              << std::endl;
    return;
  }

  // First, create the SongKey:
  auto pathKey = normalized_path(song);
  Shared::SongKey skey = "S" + std::to_string(path_to_songkey.size());
  path_to_songkey[pathKey] = skey;
  songkey_to_path[skey] = pathKey;

  // Moving on to Artists:
  std::vector<Shared::ArtistKey> artistsIds;
  for (auto& artistName : md->artist) {
    artistsIds.push_back(getOrCreateArtist(artistName));
  }
  std::vector<Shared::ArtistKey> secondaryIds;
  for (auto& artistName : md->moreArtists) {
    secondaryIds.push_back(getOrCreateArtist(artistName));
  }
  // Album:
  Shared::AlbumKey albumId =
      getOrCreateAlbum(md->album, md->year, md->artist, md->vaType);
  // Finally, the Song itself:
  Shared::SongWithPath songEntry;
  songEntry.key = skey;
  songEntry.track = md->track;
  songEntry.title = md->title;
  songEntry.albumId = albumId;
  songEntry.artistIds = artistsIds;
  songEntry.secondaryIds = secondaryIds;
  songEntry.variations = md->variations;
  songEntry.path = pathKey;
  songs[skey] = songEntry;

  // Now wire up the song to the album and the artist(s):
  auto& album = albums[albumId];
  album.songs.push_back(skey);

  for (const auto& aid : artistsIds) {
    auto& artist = artists[aid];
    artist.songs.push_back(skey);
  }
  for (const auto& aid : secondaryIds) {
    auto& artist = artists[aid];
    artist.songs.push_back(skey);
  }
}

Shared::MusicDatabase MusicDatabase::getDatabase() const {
  Shared::MusicDatabase db;
  db.songs = songs;
  db.albums = albums;
  db.artists = artists;
  // NYI: playlists
  return db;
}

} // namespace musicdb