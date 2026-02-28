#if !defined(MUSICDB_HPP)
#define MUSICDB_HPP
#pragma once

#include <cctype>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <crow.h>

#include "CommonTypes.hpp"
#include "fileindex.hpp"
#include "metadata.hpp"
#include "tuple_hash.hpp"

namespace musicdb {

using AlbumTriple = std::tuple<std::string, std::int16_t, std::string>;

class MusicDatabase {
  // To start with, the MusicDB will just be a wrapper around a single
  // file_index class, maintaining the full database in memory.
  // The metadata store will be used for metadata overrides and caching.
  // Later, we can expand this to multiple indexes (e.g., for different
  // locations, or different types of media).
  file_index* audioIndex;
  metadata::store* metadata_cache;

  std::hash<std::string> hasher;

  std::unordered_map<std::string, Shared::SongKey> path_to_songkey;
  std::unordered_map<Shared::SongKey, std::filesystem::path> songkey_to_path;

  std::unordered_map<Shared::SongKey, Shared::Song> songs;
  std::unordered_map<Shared::ArtistKey, Shared::Artist> artists;
  std::unordered_map<Shared::AlbumKey, Shared::Album> albums;

  std::unordered_map<std::string, Shared::ArtistKey> artist_name_to_key;
  std::unordered_map<AlbumTriple, Shared::AlbumKey, TupleHash>
      album_year_artist_to_key;

  std::uint32_t song_key_counter = 0;
  std::uint32_t artist_key_counter = 0;
  std::uint32_t album_key_counter = 0;
  std::string getNewSongKey();
  std::string getNewArtistKey();
  std::string getNewAlbumKey();

  static std::string normalized_path(const std::filesystem::path& p);

  // Helpers for adding items to the DB
  void addSongToDB(const std::filesystem::path& item);
  Shared::ArtistKey getOrCreateArtist(const std::string& artistName);
  Shared::AlbumKey getOrCreateAlbum(const std::string& title,
                                    std::int16_t year,
                                    const std::vector<std::string>& artists,
                                    Shared::VAType vaType);

 public:
  MusicDatabase() : audioIndex(nullptr), metadata_cache(nullptr) {}
  ~MusicDatabase();

  // Database API
  std::optional<Shared::SongKey> getSongFromPath(
      const std::filesystem::path& filepath);
  std::optional<Shared::SongWithPath> getSong(const Shared::SongKey& key);
  std::optional<std::filesystem::path> getSongPath(const Shared::SongKey& key);
  std::optional<Shared::Album> getAlbum(const Shared::AlbumKey& key);
  std::optional<Shared::Artist> getArtist(const Shared::ArtistKey& key);

  Shared::SearchResults searchIndex(bool substring, std::string& term);

  // "Implied" File Index stuff
  bool addFileLocation(const std::filesystem::path& str);
  bool removeFileLocation(const std::filesystem::path& str);
  std::vector<std::filesystem::path> getLocations() const;

  // Pictures
  void getAlbumPicture(const Shared::AlbumKey& key) const;
  void setAlbumPicture(const Shared::AlbumKey& key, const void* filepath);
  void getArtistPicture(const Shared::ArtistKey& key) const;
  void setArtistPicture(const Shared::ArtistKey& key, const void* filepath);
  void getSongPicture(const Shared::SongKey& key) const;
  void setSongPicture(const Shared::SongKey& key, const void* filepath);
  void clearImageCache();

  // Not sure about exposing these three
  // addSongFromPath(filepath: string): void; // Some Testing
  // delSongByPath(filepath: string): boolean; // Some Testing
  // delSongByKey(key: SongKey): boolean; // Some Testing

  // Ignoring stuff:
  void addIgnoreItem(Shared::IgnoreItemPair item);
  bool removeIgnoreItem(Shared::IgnoreItemPair item);
  std::vector<Shared::IgnoreItemPair> getIgnoreItems() const;

  // For all the 'parsed' data
  Shared::MusicDatabase getDatabase() const;

  // Loading from/saving to persistence
  bool load();
  bool save();

  // Updating
  bool refresh();

  // Metadata
  bool updateMetadata(const std::filesystem::path& fullPath,
                      const Shared::FullMetadata& newMetadata);
  Shared::FullMetadata getMetadata(const std::string& fullPathOrKey) const;
  std::string getCanonicalFileName(const Shared::SongKey& song) const;
  void clearMetadataCache();
  void clearLocalMetadataOverrides();
};

} // namespace musicdb

#endif // MUSICDB_HPP