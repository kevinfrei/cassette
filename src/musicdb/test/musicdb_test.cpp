#include <filesystem>
#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "musicdb.hpp"

namespace fs = std::filesystem;

fs::path self{__FILE__};

TEST(MusicDatabase, EmptyDatabase) {
  auto& db = musicdb::MusicDatabase::get();
  auto flat = db.get_flat_database();
  EXPECT_EQ(flat.songs.size(), 0);
  EXPECT_EQ(flat.albums.size(), 0);
  EXPECT_EQ(flat.artists.size(), 0);
}

TEST(MusicDatabase, AddFileLocation) {
  auto& db = musicdb::MusicDatabase::get();
  auto testDir = self.parent_path() / "fileindex" / "test" / "fileindex";
  bool added = db.add_file_location(testDir);
  EXPECT_TRUE(added);
  auto locations = db.get_locations();
  EXPECT_EQ(locations.size(), 1);
  EXPECT_EQ(locations[0], testDir);
}

TEST(MusicDatabase, GetFlatDatabase) {
  musicdb::MusicDatabase::set_locations({});
  auto& db = musicdb::MusicDatabase::get();
  auto testDir = self.parent_path() / "fileindex" / "test" / "fileindex";
  musicdb::MusicDatabase::set_locations({testDir});
  auto flat = db.get_flat_database();
  EXPECT_EQ(flat.songs.size(), 6);
  EXPECT_EQ(flat.albums.size(), 2);
  EXPECT_EQ(flat.artists.size(), 1);
}
