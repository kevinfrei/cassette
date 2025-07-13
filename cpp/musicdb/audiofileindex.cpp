#include <cstdlib>
#include <filesystem>
#include <shared_mutex>
#include <sstream>
#include <type_traits>

// Initial implementation: Just read all the files in the directory
// and produce the music-db map.

#include "CommonTypes.hpp"
#include "audiofileindex.hpp"

namespace afi {

#if defined(_WIN32)
const char* user_root = "USERPROFILE";
#elif defined(__APPLE__) || defined(__linux__)
const char* user_root = "HOME";
#else
#error Unsupported platform
#endif

namespace {
std::shared_mutex music_db_mutex;
std::unordered_set<std::string> extensions = {
    ".mp3", ".flac", ".m4a", ".jpg", ".png"};
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
  std::string home = getenv(user_root);
  std::filesystem::path root = std::filesystem::path(home) / "Music";
  if (!std::filesystem::exists(root)) {
    std::cerr << "Music directory does not exist: " << root.string()
              << std::endl;
    return nullptr;
  }
  auto db = new Shared::MusicDatabase();
  /*
  std::filesystem::recursive_directory_iterator it(root);
  std::filesystem::recursive_directory_iterator end;
  for (; it != end; ++it) {
    if (it->is_regular_file()) {
      auto path = it->path();
      auto ext = path.extension();
      if (if (extensions.find(path.extension() == ".mp3" || path.extension() ==
  ".flac" || path.extension() == ".m4a") {
      }
    }
  }
  */
  // TODO: Temporary data for testing:
  Shared::ArtistKey arist_oneday = {"r1"};
  Shared::ArtistKey artist_chv = {"r2"};
  Shared::AlbumKey album_oneday = {"a1"};
  Shared::AlbumKey album_bones = {"a2"};
  Shared::AlbumKey album_eye = {"a3"};

  Shared::Song oo1 = {//.path = "O/One Day as a Lion - 2008 - One Day as a
                      // Lion/01 - Wild International.mp3",
                      .key = Shared::SongKey{"s1"},
                      .track = 1,
                      .title = "Wild International",
                      .albumId = album_oneday,
                      .artistIds = {arist_oneday},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song oo2 = {//.path = "O/One Day as a Lion - 2008 - One Day as a
                      // Lion/02 - Ocean View.mp3",
                      .key = Shared::SongKey{"s2"},
                      .track = 2,
                      .title = "Ocean View",
                      .albumId = album_oneday,
                      .artistIds = {arist_oneday},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song oo3 = {//.path = "O/One Day as a Lion - 2008 - One Day as a
                      // Lion/03 - Last Letter.mp3",
                      .key = Shared::SongKey{"s3"},
                      .track = 3,
                      .title = "Last Letter",
                      .albumId = album_oneday,
                      .artistIds = {arist_oneday},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song oo4 = {//.path = "O/One Day as a Lion - 2008 - One Day as a
                      // Lion/04 - If You Fear Dying.mp3",
                      .key = Shared::SongKey{"s4"},
                      .track = 4,
                      .title = "If You Fear Dying",
                      .albumId = album_oneday,
                      .artistIds = {arist_oneday},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song oo5 = {//.path = "O/One Day as a Lion - 2008 - One Day as a
                      // Lion/05 - One Day As A Lion.mp3",
                      .key = Shared::SongKey{"s5"},
                      .track = 5,
                      .title = "One Day As A Lion",
                      .albumId = album_oneday,
                      .artistIds = {arist_oneday},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song cb1 = {//.path = "C/CHVRCHES/2013 - The Bones of What You
                      // Believe/01 - The Mother We Share.flac",
                      .key = Shared::SongKey{"s6"},
                      .track = 1,
                      .title = "The Mother We Share",
                      .albumId = album_bones,
                      .artistIds = {artist_chv},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song cb2 = {//.path = "C/CHVRCHES/2013 - The Bones of What You
                      // Believe/02 - We Sink.flac",
                      .key = Shared::SongKey{"s7"},
                      .track = 2,
                      .title = "We Sink",
                      .albumId = album_bones,
                      .artistIds = {artist_chv},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song cb3 = {// .path = "C/CHVRCHES/2013 - The Bones of What You
                      // Believe/03 - Gun.flac",
                      .key = Shared::SongKey{"s8"},
                      .track = 3,
                      .title = "Gun",
                      .albumId = album_bones,
                      .artistIds = {artist_chv},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song cb4 = {//.path = "C/CHVRCHES/2013 - The Bones of What You
                      // Believe/04 - Tether.flac",
                      .key = Shared::SongKey{"s9"},
                      .track = 4,
                      .title = "Tether",
                      .albumId = album_bones,
                      .artistIds = {artist_chv},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song cb5 = {//.path = "C/CHVRCHES/2013 - The Bones of What You
                      // Believe/05 - Lies.flac",
                      .key = Shared::SongKey{"s10"},
                      .track = 5,
                      .title = "Lies",
                      .albumId = album_bones,
                      .artistIds = {artist_chv},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song ce1 = {//.path = "C/CHVRCHES/2015 - Every Open Eye/01 - Never
                      // Ending Circles.flac",
                      .key = Shared::SongKey{"s11"},
                      .track = 1,
                      .title = "Never Ending Circles",
                      .albumId = album_eye,
                      .artistIds = {artist_chv},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song ce2 = {
      //.path = "C/CHVRCHES/2015 - Every Open Eye/02 - Leave a Trace.flac",
      .key = Shared::SongKey{"s12"},
      .track = 2,
      .title = "Leave a Trace",
      .albumId = album_eye,
      .artistIds = {artist_chv},
      .secondaryIds = {},
      .variations = {}};
  Shared::Song ce3 = {//.path = "C/CHVRCHES/2015 - Every Open Eye/03 - Keep You
                      // On My Side.flac",
                      .key = Shared::SongKey{"s13"},
                      .track = 3,
                      .title = "Keep You On My Side",
                      .albumId = album_eye,
                      .artistIds = {artist_chv},
                      .secondaryIds = {},
                      .variations = {}};
  Shared::Song ce4 = {
      //.path = "C/CHVRCHES/2015 - Every Open Eye/04 - Make Them Gold.flac",
      .key = Shared::SongKey{"s14"},
      .track = 4,
      .title = "Make Them Gold",
      .albumId = album_eye,
      .artistIds = {artist_chv},
      .secondaryIds = {},
      .variations = {}};
  Shared::Album l_oneday = {
      .key = album_oneday,
      .year = 2008,
      .title = "One Day as a Lion",
      .vatype = Shared::VAType::none,
      .primaryArtists = {arist_oneday},
      .songs = {oo1.key, oo2.key, oo3.key, oo4.key, oo5.key},
      .diskNames = {}};
  Shared::Album l_bones = {
      .key = album_bones,
      .year = 2013,
      .title = "The Bones of What You Believe",
      .vatype = Shared::VAType::none,
      .primaryArtists = {artist_chv},
      .songs = {cb1.key, cb2.key, cb3.key, cb4.key, cb5.key},
      .diskNames = {}};
  Shared::Album l_eye = {.key = album_eye,
                         .year = 2015,
                         .title = "Every Open Eye",
                         .vatype = Shared::VAType::none,
                         .primaryArtists = {artist_chv},
                         .songs = {ce1.key, ce2.key, ce3.key, ce4.key},
                         .diskNames = {}};
  Shared::Artist a_oneday = {
      .key = arist_oneday,
      .name = "One Day as a Lion",
      .albums = {album_oneday},
      .songs = {oo1.key, oo2.key, oo3.key, oo4.key, oo5.key}};
  Shared::Artist a_chv = {.key = artist_chv,
                          .name = "CHVRCHES",
                          .albums = {album_bones, album_eye},
                          .songs = {cb1.key,
                                    cb2.key,
                                    cb3.key,
                                    cb4.key,
                                    cb5.key,
                                    ce1.key,
                                    ce2.key,
                                    ce3.key,
                                    ce4.key}};
  db->artists[a_oneday.key] = a_oneday;
  db->artists[a_chv.key] = a_chv;
  db->albums[l_oneday.key] = l_oneday;
  db->albums[l_bones.key] = l_bones;
  db->albums[l_eye.key] = l_eye;
  db->songs[oo1.key] = oo1;
  db->songs[oo2.key] = oo2;
  db->songs[oo3.key] = oo3;
  db->songs[oo4.key] = oo4;
  db->songs[oo5.key] = oo5;
  db->songs[cb1.key] = cb1;
  db->songs[cb2.key] = cb2;
  db->songs[cb3.key] = cb3;
  db->songs[cb4.key] = cb4;
  db->songs[cb5.key] = cb5;
  db->songs[ce1.key] = ce1;
  db->songs[ce2.key] = ce2;
  db->songs[ce3.key] = ce3;
  db->songs[ce4.key] = ce4;
  return music_db = db;
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

} // namespace afi