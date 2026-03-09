#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <set>
#include <shared_mutex>

#include "CommonTypes.hpp"

#include "musicdb.hpp"

namespace fs = std::filesystem;

namespace musicdb {

namespace {

std::shared_mutex music_db_mutex;
std::array<std::string, 5> extensions = {
    ".mp3", ".flac", ".m4a", ".jpg", ".png"};
std::array<std::string, 3> audio_ext = {".mp3", ".flac", ".m4a"};

} // namespace

void set_locations(const std::vector<fs::path>& locations) {
  MusicDatabase::set_locations(locations);
}

MusicDatabase& MusicDatabase::get() {
  static MusicDatabase singleton;
  return singleton;
};

void MusicDatabase::set_locations(const std::vector<fs::path>& locations) {
  // Remove any current locations that aren't in the list, just to be safe
  MusicDatabase& mdb = get();
  std::set<fs::path> loc_set{locations.cbegin(), locations.cend()};
  for (const auto& cur_loc : mdb.get_locations()) {
    if (loc_set.find(cur_loc) != loc_set.end()) {
      mdb.remove_file_location(cur_loc);
    }
  }
  // Add all the locations. The API should be resilient to duplicate locations.
  for (const auto& loc : locations) {
    mdb.add_file_location(loc);
  }
}

MusicDatabase::MusicDatabase() {}

MusicDatabase::~MusicDatabase() {
  if (metadata_cache) {
    delete metadata_cache;
    metadata_cache = nullptr;
  }
}

std::string MusicDatabase::get_new_song_key() {
  return "S" + std::to_string(song_key_counter++);
}

std::string MusicDatabase::get_new_artist_key() {
  return "R" + std::to_string(artist_key_counter++);
}

std::string MusicDatabase::get_new_album_key() {
  return "L" + std::to_string(album_key_counter++);
}

bool MusicDatabase::add_file_location(const std::filesystem::path& root) {
  if (std::find_if(
          audio_index.cbegin(), audio_index.cend(), [&](const file_index& i) {
            return i.get_location() == root;
          }) != audio_index.cend()) {
    return false;
  }
  audio_index.push_back(file_index{root, true});
  auto& ai = *audio_index.rend();
  ai.foreach_file([this](const fs::path& p) {
    std::string ext = p.extension().string();
    for (const auto& validExt : audio_ext) {
      if (ext == validExt) {
        add_song_to_db(p);
        break;
      }
    }
  });
  return true;
}

std::optional<Shared::Album> MusicDatabase::get_album_from_key(
    const Shared::AlbumKey& key) {
  auto maybe = albums.find(key);
  if (maybe == albums.end()) {
    return std::nullopt;
  }
  return maybe->second;
}

std::optional<Shared::SongWithPath> MusicDatabase::get_song(
    const Shared::SongKey& key) {
  auto maybe = songs.find(key);
  if (maybe == songs.end()) {
    return std::nullopt;
  }
  return maybe->second;
}

std::optional<Shared::AlbumKey> MusicDatabase::get_album(
    const std::string& title,
    std::int16_t year,
    const std::vector<std::string>& artists,
    Shared::VAType vaType) {
  return get_album_helper(title, year, artists, vaType).first;
}

std::pair<std::optional<Shared::AlbumKey>, AlbumTriple>
MusicDatabase::get_album_helper(const std::string& title,
                                std::int16_t year,
                                const std::vector<std::string>& artists,
                                Shared::VAType vaType) {
  AlbumTriple keyTuple = make_album_triple(title, year, artists, vaType);
  auto it = album_year_artist_to_key.find(keyTuple);
  if (it != album_year_artist_to_key.end()) {
    return std::make_pair(it->second, keyTuple);
  }
  return std::make_pair(std::nullopt, keyTuple);
}

bool MusicDatabase::remove_file_location(const std::filesystem::path&) {
  // NYI
  return false;
}

std::vector<std::filesystem::path> MusicDatabase::get_locations() const {
  std::vector<std::filesystem::path> locations;
  locations.reserve(audio_index.size());
  for (const auto& value : audio_index) {
    locations.push_back(value.get_location());
  }
  return locations;
}

std::string MusicDatabase::normalized_path(const std::filesystem::path& p) {
  return std::filesystem::weakly_canonical(p).generic_string();
}

Shared::ArtistKey MusicDatabase::get_or_create_artist(
    const std::string& artistName) {
  // TODO: Make this case-insensitive
  auto it = artist_name_to_key.find(artistName);
  if (it != artist_name_to_key.end()) {
    return it->second;
  }

  Shared::ArtistKey newKey = get_new_artist_key();
  artist_name_to_key[artistName] = newKey;
  Shared::Artist artistEntry;
  artistEntry.key = newKey;
  artistEntry.name = artistName;
  artists[newKey] = artistEntry;
  return newKey;
}

AlbumTriple MusicDatabase::make_album_triple(
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
  return AlbumTriple{title, year, artistHashValue};
}

Shared::AlbumKey MusicDatabase::get_or_create_album(
    const std::string& title,
    std::int16_t year,
    const std::vector<std::string>& artists,
    Shared::VAType vaType) {
  auto [existing_album, keyTuple] =
      get_album_helper(title, year, artists, vaType);
  if (existing_album) {
    return *existing_album;
  }

  Shared::AlbumKey newKey = get_new_album_key();
  album_year_artist_to_key[keyTuple] = newKey;
  Shared::Album albumEntry;
  albumEntry.key = newKey;
  albumEntry.year = year;
  albumEntry.title = title;
  albumEntry.vatype = vaType;
  for (const auto& artist : artists) {
    Shared::ArtistKey artistKey = get_or_create_artist(artist);
    albumEntry.primaryArtists.push_back(artistKey);
    // Now, add this album to the artist's list of albums:
    Shared::Artist& theArtist = this->artists[artistKey];
    theArtist.albums.push_back(newKey);
  }
  albums[newKey] = albumEntry;
  return newKey;
}

void MusicDatabase::add_song_to_db(const fs::path& song) {
  // First, get the metadata for the song
  auto md = metadata_cache->read(song);
  if (!md) {
    std::cerr << "Failed to get metadata for song: " << song.string()
              << std::endl;
    return;
  }

  // First, create the SongKey:
  auto pathKey = normalized_path(song);
  Shared::SongKey skey = get_new_song_key();
  path_to_songkey[pathKey] = skey;

  // Moving on to Artists:
  std::vector<Shared::ArtistKey> artistsIds;
  for (auto& artistName : md->artist) {
    artistsIds.push_back(get_or_create_artist(artistName));
  }
  std::vector<Shared::ArtistKey> secondaryIds;
  for (auto& artistName : md->moreArtists) {
    secondaryIds.push_back(get_or_create_artist(artistName));
  }
  // Album:
  Shared::AlbumKey albumId =
      get_or_create_album(md->album, md->year, md->artist, md->vaType);
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
  album.songs.insert(std::make_pair(songEntry.track, skey));

  for (const auto& aid : artistsIds) {
    auto& artist = artists[aid];
    artist.songs.push_back(skey);
  }
  for (const auto& aid : secondaryIds) {
    auto& artist = artists[aid];
    artist.songs.push_back(skey);
  }
}

std::optional<std::filesystem::path> MusicDatabase::get_song_path(
    const Shared::SongKey& key) {
  auto it = songs.find(key);
  if (it == songs.end()) {
    return std::nullopt;
  }
  return it->second.path;
}

const Shared::MusicDatabase& MusicDatabase::get_flat_database() {
  flat_db.songs.clear();
  flat_db.albums.clear();
  flat_db.artists.clear();
  flat_db.songs.insert(songs.begin(), songs.end());
  flat_db.albums = albums;
  flat_db.artists = artists;
  // NYI: playlists
  return flat_db;
}

} // namespace musicdb