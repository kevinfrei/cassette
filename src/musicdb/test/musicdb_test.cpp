#include <filesystem>
#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "musicdb.hpp"

namespace fs = std::filesystem;

fs::path self{__FILE__};

class MDBTest : public ::testing::Test {

 protected:
  MDBTest() {}

  virtual ~MDBTest() {}
  // SetUp() method for per-test setup
  void SetUp() override {
    std::cout << "ItsATest Fixture SetUp called." << std::endl;
    // Per-test setup code here
  }

  // TearDown() method for per-test cleanup
  void TearDown() override {
    std::cout << "ItsATest Fixture TearDown called." << std::endl;
    // This code will run after each test using this fixture,
    // regardless of whether the test passed or failed.
    // For example, close files, release resources, etc.
  }
};

TEST_F(MDBTest, EmptyDatabase) {
  auto& db = musicdb::MusicDatabase::get();
  auto flat = db.get_flat_database();
  EXPECT_EQ(flat.songs.size(), 0);
  EXPECT_EQ(flat.albums.size(), 0);
  EXPECT_EQ(flat.artists.size(), 0);
}

TEST_F(MDBTest, AddFileLocation) {
  auto& db = musicdb::MusicDatabase::get();
  auto testDir =
      self.parent_path().parent_path() / "fileindex" / "test" / "fileindex";
  bool added = db.add_file_location(testDir);
  EXPECT_TRUE(added);
  auto locations = db.get_locations();
  EXPECT_EQ(locations.size(), 1);
  EXPECT_EQ(fs::canonical(locations[0]), fs::canonical(testDir));
}

TEST_F(MDBTest, GetFlatDatabase) {
  musicdb::MusicDatabase::set_locations({});
  auto& db = musicdb::MusicDatabase::get();
  auto testDir =
      self.parent_path().parent_path() / "fileindex" / "test" / "fileindex";
  musicdb::MusicDatabase::set_locations({testDir});
  auto flat = db.get_flat_database();
  EXPECT_EQ(flat.artists.size(), 2);
  EXPECT_EQ(flat.albums.size(), 3);
  EXPECT_EQ(flat.songs.size(), 6);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
