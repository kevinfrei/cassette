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
  std::vector<file_index> audio_index;
  metadata::store* metadata_cache;
  void init_md_store();

  std::hash<std::string> hasher;

  // Stuff for the index:
  std::unordered_map<std::string, Shared::SongKey> path_to_songkey;
  std::unordered_map<Shared::SongKey, Shared::SongWithPath> songs;
  std::unordered_map<Shared::ArtistKey, Shared::Artist> artists;
  std::unordered_map<Shared::AlbumKey, Shared::Album> albums;
  std::unordered_map<std::string, Shared::ArtistKey> artist_name_to_key;
  std::unordered_map<AlbumTriple, Shared::AlbumKey, TupleHash>
      album_year_artist_to_key;
  Shared::MusicDatabase flat_db;

  std::uint32_t song_key_counter = 0;
  std::uint32_t artist_key_counter = 0;
  std::uint32_t album_key_counter = 0;
  std::string get_new_song_key();
  std::string get_new_artist_key();
  std::string get_new_album_key();

  static std::string normalized_path(const std::filesystem::path& p);

  // Helpers for adding items to the DB
  void add_song_to_db(const std::filesystem::path& item);
  Shared::ArtistKey get_or_create_artist(const std::string& artistName);
  std::optional<Shared::ArtistKey> get_artist(const std::string& artistName);
  Shared::AlbumKey get_or_create_album(const std::string& title,
                                       std::int16_t year,
                                       const std::vector<std::string>& artists,
                                       Shared::VAType vaType);

  MusicDatabase();
  ~MusicDatabase();
  MusicDatabase(const MusicDatabase&) = delete; // Delete copy constructor
  MusicDatabase& operator=(const MusicDatabase&) = delete; // Delete assignment

 public:
  static MusicDatabase& get();

  // Database API
  std::optional<Shared::SongKey> get_song_from_path(
      const std::filesystem::path& filepath);
  std::optional<Shared::SongWithPath> get_song(const Shared::SongKey& key);
  std::optional<std::filesystem::path> get_song_path(
      const Shared::SongKey& key);
  std::optional<Shared::Album> get_album_from_key(const Shared::AlbumKey& key);
  AlbumTriple make_album_triple(const std::string& title,
                                std::int16_t year,
                                const std::vector<std::string>& artists,
                                Shared::VAType vaType);
  std::optional<Shared::Artist> get_artist_from_key(
      const Shared::ArtistKey& key);
  std::optional<Shared::AlbumKey> get_album(
      const std::string& title,
      std::int16_t year,
      const std::vector<std::string>& artists,
      Shared::VAType vaType);
  std::pair<std::optional<Shared::AlbumKey>, AlbumTriple> get_album_helper(
      const std::string& title,
      std::int16_t year,
      const std::vector<std::string>& artists,
      Shared::VAType vaType);

  Shared::SearchResults search_index(bool substring, std::string& term);

  // "Implied" File Index stuff
  static void set_locations(
      const std::vector<std::filesystem::path>& locations);
  bool add_file_location(const std::filesystem::path& str);
  bool remove_file_location(const std::filesystem::path& str);
  std::vector<std::filesystem::path> get_locations() const;

  // Pictures
  std::optional<std::filesystem::path> get_album_picture(
      const Shared::AlbumKey& key) const;
  void set_album_picture(const Shared::AlbumKey& key,
                         const std::filesystem::path& filepath);
  std::optional<std::filesystem::path> get_artist_picture(
      const Shared::ArtistKey& key) const;
  void set_artist_picture(const Shared::ArtistKey& key,
                          const std::filesystem::path& filepath);
  std::optional<std::filesystem::path> get_song_picture(
      const Shared::SongKey& key) const;
  void set_song_picture(const Shared::SongKey& key,
                        const std::filesystem::path& filepath);
  void clear_image_cache();

  // Not sure about exposing these three
  // addSongFromPath(filepath: string): void; // Some Testing
  // delSongByPath(filepath: string): boolean; // Some Testing
  // delSongByKey(key: SongKey): boolean; // Some Testing

  // Ignoring stuff:
  void add_ignore_itemm(Shared::IgnoreItemPair item);
  bool remove_ignore_item(Shared::IgnoreItemPair item);
  std::vector<Shared::IgnoreItemPair> get_ignore_items() const;

  // For all the 'parsed' data
  const Shared::MusicDatabase& get_flat_database();

  // Loading from/saving to persistence
  bool load();
  bool save();

  // Updating
  bool refresh();

  // Metadata
  bool update_metadata(const std::filesystem::path& fullPath,
                       const Shared::FullMetadata& newMetadata);
  Shared::FullMetadata get_metadata(const std::string& fullPathOrKey) const;
  std::string get_canonical_file_name(const Shared::SongKey& song) const;
  void clear_metadata_cache();
  void clear_local_metadata_overrides();
};

inline MusicDatabase& get() {
  return MusicDatabase::get();
}

} // namespace musicdb
