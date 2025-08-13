#include <filesystem>
#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "fileindex.hpp"

namespace fs = std::filesystem;
const char* dir_name = "fileindex";

fs::path self{__FILE__};

class AFI : public ::testing::Test {
  // delete the .afi subdirectory before and after running the tests.
  void rd(const fs::path& dir) {
    if (fs::exists(dir)) {
      fs::remove_all(dir);
    }
  }
  void rm(const fs::path& file) {
    if (fs::exists(file)) {
      fs::remove(file);
    }
  }
  void remove_stuff() {
    auto testDir = self.parent_path();
    rd(testDir / dir_name / ".afi");
    rm(testDir / dir_name /
       "Test Artist - 2010 - Test Album/04 - New File Not There.mp3");
  }
  void backup_index() {
    auto testDir = self.parent_path();
    auto indexFile = testDir / "NotActuallyFiles" / ".afi" / "index.txt";
    if (fs::exists(indexFile)) {
      fs::copy(indexFile,
               indexFile.string() + ".bak",
               fs::copy_options::overwrite_existing);
    }
  }
  void restore_index() {
    auto testDir = self.parent_path();
    auto indexFile = testDir / "NotActuallyFiles" / ".afi" / "index.txt";
    auto backupFile = indexFile.string() + ".bak";
    if (fs::exists(backupFile)) {
      fs::copy(backupFile, indexFile, fs::copy_options::overwrite_existing);
      fs::remove(backupFile);
    }
  }

 protected:
  AFI() {
    backup_index();
    remove_stuff();
  }

  ~AFI() override {
    restore_index();
    remove_stuff();
  }
  /*
  // SetUp() method for per-test setup
  void SetUp() override {
    std::cout << "AFI Fixture SetUp called." << std::endl;
    // Per-test setup code here
  }

  // TearDown() method for per-test cleanup
  void TearDown() override {
    std::cout << "AFI Fixture TearDown called." << std::endl;
    // This code will run after each test using this fixture,
    // regardless of whether the test passed or failed.
    // For example, close files, release resources, etc.
  }
  */
};

TEST_F(AFI, SmallFileIndex_basics) {
  std::chrono::time_point<std::chrono::system_clock> start =
      std::chrono::system_clock::now();
  auto afi = afi::audio_file_index{self.parent_path() / dir_name};
  EXPECT_NE(afi.get_hash(), 0);
  auto p = afi.get_location();
  // std::cout << p.generic_string() << std::endl;
  EXPECT_FALSE(p.generic_string().ends_with("."));
  int i = 0;
  afi.foreach_audio_file([&](const fs::path& path) {
    // std::cout << "Audio file: " << path.generic_string() << std::endl;
    EXPECT_TRUE(path.is_absolute());
    EXPECT_TRUE(path.has_filename());
    EXPECT_TRUE(path.has_extension());
    EXPECT_TRUE(path.extension().generic_string() == ".mp3" ||
                path.extension().generic_string() == ".flac");
    EXPECT_TRUE(path.generic_string().find("/01 - ") != std::string::npos ||
                path.generic_string().find("/02 - ") != std::string::npos);
    i++;
  });
  // There should be a total of 6 files in the test directory.
  EXPECT_EQ(i, 6);
  // Check the timestamp of the last scan.
  auto lastScan = afi.get_last_scan_time();
  EXPECT_GT(lastScan, start);
}

TEST_F(AFI, SmallFileIndex_metadata) {
  auto afi = afi::audio_file_index{self.parent_path() / dir_name};
  EXPECT_NE(afi.get_hash(), 0);
  auto p = afi.get_location();
  // std::cout << p.generic_string() << std::endl;
  EXPECT_FALSE(p.generic_string().ends_with("."));
  int i = 0;
  afi.foreach_audio_file([&](const fs::path& path) {
    // std::cout << "Audio file: " << path.generic_string() << std::endl;
    EXPECT_TRUE(path.is_absolute());
    EXPECT_TRUE(path.has_filename());
    EXPECT_TRUE(path.has_extension());
    EXPECT_TRUE(path.extension().generic_string() == ".mp3" ||
                path.extension().generic_string() == ".flac");
    i++;
    auto metadata = afi.get_metadata_from_path(path);
    EXPECT_TRUE(metadata.has_value());
    if (metadata.has_value()) {
      // std::cout << "Metadata: " << metadata->title << std::endl;
      EXPECT_TRUE(path.generic_string().ends_with(metadata->originalPath));
      EXPECT_FALSE(metadata->title.empty());
      EXPECT_EQ(metadata->artist.size(), 1);
      EXPECT_TRUE(metadata->artist[0].ends_with("Artist"));
      EXPECT_FALSE(metadata->album.empty());
      EXPECT_NE(metadata->album.find("Album"), std::string::npos);
      EXPECT_GT(metadata->year, 1990);
      EXPECT_LT(metadata->year, 2026);
      EXPECT_GT(metadata->track, 0);
      EXPECT_LT(metadata->track, 3);
      EXPECT_EQ(metadata->vaType, Shared::VAType::none);
      EXPECT_TRUE(metadata->diskName.empty());
      EXPECT_EQ(metadata->disk, 0);
      EXPECT_TRUE(metadata->moreArtists.empty());
      EXPECT_TRUE(metadata->variations.empty());
    }
  });
  // There should be a total of 6 files in the test directory.
  EXPECT_EQ(i, 6);
}
TEST_F(AFI, LargeFileIndex) {
  auto afi =
      afi::audio_file_index{self.parent_path() / "NotActuallyFiles", false};
  EXPECT_NE(afi.get_hash(), 0);
  EXPECT_EQ(afi.get_last_scan_time(),
            std::chrono::system_clock::time_point::min());
  auto p = afi.get_location();
  int i = 0;
  int mp3 = 0;
  int m4a = 0;
  int jpg = 0;
  int flac = 0;
  afi.foreach_audio_file([&](const fs::path& path) {
    // std::cout << "Audio file: " << path.generic_string() << std::endl;
    EXPECT_TRUE(path.is_absolute());
    EXPECT_TRUE(path.has_filename());
    EXPECT_TRUE(path.has_extension());
    auto ext = path.extension().generic_string();
    bool isMp3 = ext == ".mp3";
    bool isM4a = ext == ".m4a";
    bool isJpg = ext == ".jpg";
    bool isFlac = ext == ".flac";
    mp3 += isMp3;
    m4a += isM4a;
    jpg += isJpg;
    flac += isFlac;
    if (!isMp3 && !isM4a && !isJpg && !isFlac) {
      std::cerr << "Unexpected file: " << path.generic_string() << std::endl;
    }
    i++;
  });
  // There should be a total of 870 files in the test directory.
  EXPECT_EQ(mp3, 739);
  EXPECT_EQ(m4a, 3);
  EXPECT_EQ(jpg, 127);
  EXPECT_EQ(flac, 1);
  EXPECT_EQ(i, mp3 + m4a + jpg + flac);
  // TODO: Update the AFI to have some changes, and check the scan times.
  int added = 0;
  int removed = 0;
  int mp3delta = 0;
  int jpgdelta = 0;
  int otherdelta = 0;
  afi.rescan_files(
      [&](const fs::path& path) {
        // std::cout << "Added file: " << path.generic_string() << std::endl;
        added++;
        if (path.extension() == ".mp3") {
          mp3delta++;
        } else if (path.extension() == ".jpg") {
          jpgdelta++;
        } else {
          otherdelta++;
        }
      },
      [&](const fs::path& path) {
        // std::cout << "Removed file: " << path.generic_string() << std::endl;
        removed++;
        if (path.extension() == ".mp3") {
          mp3delta--;
        } else if (path.extension() == ".jpg") {
          jpgdelta--;
        } else {
          otherdelta--;
        }
      });
  EXPECT_GT(afi.get_last_scan_time(),
            std::chrono::system_clock::time_point::min());
  EXPECT_LE(afi.get_last_scan_time(), std::chrono::system_clock::now());
  EXPECT_EQ(added, 2);
  EXPECT_EQ(removed, 2);
  EXPECT_EQ(mp3delta, 0);
  EXPECT_EQ(jpgdelta, 0);
  EXPECT_EQ(otherdelta, 0);
}