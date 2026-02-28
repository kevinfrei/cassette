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

namespace musicdb {

namespace {

std::shared_mutex music_db_mutex;
std::array<std::string, 5> extensions = {
    ".mp3", ".flac", ".m4a", ".jpg", ".png"};
std::array<std::string, 3> audio_ext = {".mp3", ".flac", ".m4a"};
MusicDatabase* mdb = nullptr;

} // namespace

void set_locations(const std::vector<fs::path>& locations) {
  std::unique_lock<std::shared_mutex> lock(music_db_mutex);
  if (mdb) {
    delete mdb;
    mdb = nullptr;
  }
  mdb = new MusicDatabase();
  for (const auto& loc : locations) {
    mdb->addFileLocation(loc);
  }
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

std::string MusicDatabase::getNewSongKey() {
  return "S" + std::to_string(song_key_counter++);
}

std::string MusicDatabase::getNewArtistKey() {
  return "R" + std::to_string(artist_key_counter++);
}

std::string MusicDatabase::getNewAlbumKey() {
  return "L" + std::to_string(album_key_counter++);
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

bool MusicDatabase::removeFileLocation(const std::filesystem::path&) {
  // NYI
  return false;
}

std::vector<std::filesystem::path> MusicDatabase::getLocations() const {
  if (audioIndex) {
    return std::vector<std::filesystem::path>{audioIndex->get_location()};
  }
  return std::vector<std::filesystem::path>{};
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

  Shared::ArtistKey newKey = getNewArtistKey();
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
  Shared::AlbumKey newKey = getNewAlbumKey();
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
  Shared::SongKey skey = getNewSongKey();
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

std::optional<std::filesystem::path> MusicDatabase::getSongPath(
    const Shared::SongKey& key) {
  auto it = songkey_to_path.find(key);
  if (it == songkey_to_path.end()) {
    return std::nullopt;
  }
  return it->second;
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