#include <filesystem>
#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "musicdb.hpp"

namespace fs = std::filesystem;

fs::path self{__FILE__};
const char* dir_name = "fileindex";
const char* large_dir_name = "NotActuallyFiles";
const char* idx_dir = ".afi";
fs::path getFilesPath() {
  return self.parent_path().parent_path() / "fileindex" / "test";
}
class MDBTest : public ::testing::Test {

 protected:
  void backup_index() {
    auto testDir = getFilesPath();
    auto indexFile = testDir / large_dir_name / idx_dir / "index.txt";
    if (fs::exists(indexFile)) {
      fs::copy(indexFile,
               indexFile.string() + ".bak",
               fs::copy_options::overwrite_existing);
    }
  }
  void restore_index() {
    auto testDir = getFilesPath();
    auto indexFile = testDir / large_dir_name / idx_dir / "index.txt";
    auto backupFile = indexFile.string() + ".bak";
    if (fs::exists(backupFile)) {
      fs::copy(backupFile, indexFile, fs::copy_options::overwrite_existing);
      fs::remove(backupFile);
    }
  }

  MDBTest() {
    backup_index();
  }

  virtual ~MDBTest() {
    restore_index();
  }

  // SetUp() method for per-test setup
  void SetUp() override {
    // std::cout << "ItsATest Fixture SetUp called." << std::endl;
    // Per-test setup code here
  }

  // TearDown() method for per-test cleanup
  void TearDown() override {
    // std::cout << "ItsATest Fixture TearDown called." << std::endl;
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
  auto testDir = getFilesPath() / dir_name;
  bool added = db.add_file_location(testDir);
  EXPECT_TRUE(added);
  auto locations = db.get_locations();
  EXPECT_EQ(locations.size(), 1);
  EXPECT_EQ(fs::canonical(locations[0]), fs::canonical(testDir));
}

TEST_F(MDBTest, GetFlatDatabase) {
  musicdb::MusicDatabase::set_locations({});
  auto& db = musicdb::MusicDatabase::get();
  auto testDir = getFilesPath() / dir_name;
  musicdb::MusicDatabase::set_locations({testDir});
  auto flat = db.get_flat_database();
  EXPECT_EQ(flat.artists.size(), 2);
  EXPECT_EQ(flat.albums.size(), 3);
  EXPECT_EQ(flat.songs.size(), 6);
}

TEST_F(MDBTest, FullDatabase) {
  musicdb::MusicDatabase::set_locations({});
  auto& db = musicdb::MusicDatabase::get();
  auto testDir = getFilesPath() / large_dir_name;
  db.add_file_location(testDir);
  auto flat = db.get_flat_database();
  EXPECT_EQ(flat.artists.size(), 269); // 273);
  EXPECT_EQ(flat.albums.size(), 189);
  EXPECT_EQ(flat.songs.size(), 742);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
