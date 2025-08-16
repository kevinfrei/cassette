#if !defined(MUSICDB_HPP)
#define MUSICDB_HPP
#pragma once

#include <crow.h>

#include "CommonTypes.hpp"
#include "fileindex.hpp"

namespace musicdb {

Shared::MusicDatabase *get_music_db();
void send_music_db(crow::websocket::connection &conn);

class MusicDatabase {
  // To start with, the MusicDB will just be a wrapper around a single
  // file_index class, maintaining the full database in memory.
  file_index *audioIndex;

public:
  MusicDatabase() : audioIndex(nullptr) {}

  MusicDatabase(file_index *index) : audioIndex(index) {
    // TODO: initialize the index if it is not already initialized
  }

  ~MusicDatabase() {
    if (audioIndex) {
      delete audioIndex;
      audioIndex = nullptr;
    }
  }

  // Database API
  Shared::SongWithPath getSong(Shared::SongKey &key);
  Shared::Album getAlbum(Shared::AlbumKey &key);
  Shared::Artist getArtist(Shared::ArtistKey &key);
  Shared::SongKey getSongFromPath(std::string &filepath);
  Shared::SearchResults searchIndex(bool substring, std::string &term);

  // Full File Index stuff
  bool addAudioFileIndex(file_index *idx);
  bool removeAudioFileIndex(const file_index *idx);

  // "Implied" File Index stuff
  bool addFileLocation(const std::string &str);
  bool removeFileLocation(const std::string &str);
  std::vector<std::string> getLocations() const;

  // Pictures
  void getAlbumPicture(const Shared::AlbumKey &key) const;
  void setAlbumPicture(const Shared::AlbumKey &key, const void *filepath);
  void getArtistPicture(const Shared::ArtistKey &key) const;
  void setArtistPicture(const Shared::ArtistKey &key, const void *filepath);
  void getSongPicture(const Shared::SongKey &key) const;
  void setSongPicture(const Shared::SongKey &key, const void *filepath);
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
  bool updateMetadata(
      const std::string &fullPath,
      const Shared::FullMetadata &newMetadata
  );
  Shared::FullMetadata getMetadata(const std::string &fullPathOrKey) const;
  std::string getCanonicalFileName(const Shared::SongKey &song) const;
  void clearMetadataCache();
  void clearLocalMetadataOverrides();
  // addOrUpdateSong(md: FullMetadata): void;
};

} // namespace musicdb

#endif // MUSICDB_HPP