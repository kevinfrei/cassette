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
  auto cur_locs = mdb.get_locations();
  std::set<fs::path> loc_new{locations.cbegin(), locations.cend()};
  std::set<fs::path> loc_cur{cur_locs.cbegin(), cur_locs.cend()};

  for (auto& to_del : loc_cur) {
    if (!loc_new.contains(to_del)) {
      mdb.remove_file_location(to_del);
    }
  }
  // Add all the locations. The API should be resilient to duplicate locations.
  for (auto& to_add : loc_new) {
    if (!loc_cur.contains(to_add)) {
      mdb.add_file_location(to_add);
    }
  }
}

MusicDatabase::MusicDatabase() {}

MusicDatabase::~MusicDatabase() {}

std::string MusicDatabase::get_new_song_key() {
  return "S" + std::to_string(song_key_counter++);
}

std::string MusicDatabase::get_new_artist_key() {
  return "R" + std::to_string(artist_key_counter++);
}

std::string MusicDatabase::get_new_album_key() {
  return "L" + std::to_string(album_key_counter++);
}

bool MusicDatabase::add_file_location(const fs::path& root) {
  if (std::find_if(audio_index.cbegin(),
                   audio_index.cend(),
                   [&](const FileIndexCache& fic) {
                     return fic.fi.get_location() == root;
                   }) != audio_index.cend()) {
    return false;
  }
  FileIndexCache fic{file_index{root, true}, metadata::store{root}};
  audio_index.emplace_back(std::move(fic));
  FileIndexCache* ficp = get_index_for_path(root);
  ficp->fi.foreach_file([&](const fs::path& p) {
    std::string ext = p.extension().string();
    for (const auto& validExt : audio_ext) {
      if (ext == validExt) {
        add_song_to_db(p, ficp);
        break;
      }
    }
  });
  return true;
}

bool is_subpath(const fs::path& path, const fs::path& base) {
  auto rel = fs::relative(path, base);
  // Returns true if relative path is valid and does not escape base
  return !rel.empty() && rel.generic_string().find("..") == std::string::npos;
}

MusicDatabase::FileIndexCache* MusicDatabase::get_index_for_path(
    const fs::path& p) {
  for (auto& fic : audio_index) {
    if (is_subpath(p, fic.fi.get_location())) {
      return &fic;
    }
  }
  return nullptr;
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
    const std::vector<std::string>& arts,
    Shared::VAType vaType) {
  return get_album_helper(title, year, arts, vaType).first;
}

std::pair<std::optional<Shared::AlbumKey>, AlbumTriple>
MusicDatabase::get_album_helper(const std::string& title,
                                std::int16_t year,
                                const std::vector<std::string>& arts,
                                Shared::VAType vaType) {
  AlbumTriple keyTuple = make_album_triple(title, year, arts, vaType);
  auto it = album_year_artist_to_key.find(keyTuple);
  if (it != album_year_artist_to_key.end()) {
    return std::make_pair(it->second, keyTuple);
  }
  return std::make_pair(std::nullopt, keyTuple);
}

bool MusicDatabase::remove_file_location(const fs::path& dir) {
  // Remove the audio index associated:
  size_t item = audio_index.size();
  for (size_t i = 0; i < item; i++) {
    if (is_subpath(dir, audio_index[i].fi.get_location())) {
      item = i;
    }
  }
  if (item == audio_index.size()) {
    return false;
  }
  FileIndexCache& fic = audio_index.at(item);
  fic.fi.foreach_file([&](const fs::path&) -> void {
    // TODO: Remove the file from the database
    return;
  });
  audio_index.erase(audio_index.begin() + item);
  return true;
}

std::vector<fs::path> MusicDatabase::get_locations() const {
  std::vector<fs::path> locations;
  locations.reserve(audio_index.size());
  for (const auto& value : audio_index) {
    locations.push_back(value.fi.get_location());
  }
  return locations;
}

std::string MusicDatabase::normalized_path(const fs::path& p) {
  return fs::weakly_canonical(p).generic_string();
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
    const std::vector<std::string>& arts,
    Shared::VAType vaType) {
  std::string artistHashValue;
  if (vaType != Shared::VAType::none) {
    // For VA albums, we can't really do a good job of deduping.
    artistHashValue = Shared::to_string(vaType);
    artistHashValue.push_back('*');
  } else {
    // TODO: Make this case-insensitive
    for (const auto& artist : arts) {
      artistHashValue += artist + "|";
    }
  }
  return AlbumTriple{title, year, artistHashValue};
}

Shared::AlbumKey MusicDatabase::get_or_create_album(
    const std::string& title,
    std::int16_t year,
    const std::vector<std::string>& arts,
    Shared::VAType vaType) {
  auto [existing_album, keyTuple] = get_album_helper(title, year, arts, vaType);
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
  for (const auto& artist : arts) {
    Shared::ArtistKey artistKey = get_or_create_artist(artist);
    albumEntry.primaryArtists.push_back(artistKey);
    // Now, add this album to the artist's list of albums:
    Shared::Artist& theArtist = this->artists[artistKey];
    theArtist.albums.push_back(newKey);
  }
  albums[newKey] = albumEntry;
  return newKey;
}

void MusicDatabase::add_song_to_db(const fs::path& song, FileIndexCache* fic) {
  // First, get the metadata for the song
  fic = fic == nullptr ? get_index_for_path(song) : fic;
  if (fic == nullptr) {
    return;
  }
  auto md = fic->cache.read(song);
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

std::optional<fs::path> MusicDatabase::get_song_path(
    const Shared::SongKey& key) {
  auto it = songs.find(key);
  if (it == songs.end()) {
    return std::nullopt;
  }
  return it->second.path;
}

const Shared::FlatMusicDatabase& MusicDatabase::get_flat_database() {
  static Shared::FlatMusicDatabase flat_db;
  // std::cout << "Building flat music database..." << std::endl;
  flat_db.songs.clear();
  flat_db.albums.clear();
  flat_db.artists.clear();
  flat_db.songs.reserve(songs.size());
  for (const auto& [key, song] : songs) {
    flat_db.songs.push_back(song);
  }
  flat_db.albums.reserve(albums.size());
  for (const auto& [key, album] : albums) {
    flat_db.albums.push_back(album);
  }
  flat_db.artists.reserve(artists.size());
  for (const auto& [key, artist] : artists) {
    flat_db.artists.push_back(artist);
  }
  // std::cout << "Done building flat music database:" << flat_db.songs.size()
  // << " songs, " << flat_db.albums.size() << " albums, " <<
  // flat_db.artists.size() << " artists." << std::endl; NYI: playlists
  return flat_db;
}

const Shared::MusicDatabase& MusicDatabase::get_music_database() {
  static Shared::MusicDatabase db;
  // std::cout << "Building music database..." << std::endl;
  db.songs.clear();
  db.albums.clear();
  db.artists.clear();
  for (const auto& [key, song] : songs) {
    db.songs[key] = song;
  }
  db.albums = albums;
  db.artists = artists;
  // std::cout << "Done building music database:" << db.songs.size() << " songs,
  // " << db.albums.size() << " albums, " << db.artists.size() << " artists." <<
  // std::endl;
  return db;
}

} // namespace musicdb
