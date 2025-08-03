#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include "CommonTypes.hpp"
#include "audiofileindex.h"

namespace fs = std::filesystem;

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
    rd(testDir / ".afi");
    rm(testDir / "audiofileindex" /
       "Test Artist - 2010 - Test Album/04 - New File Not There.mp3");
  }

 protected:
  AFI() {
    remove_stuff();
  }

  ~AFI() override {
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

TEST_F(AFI, SmallFileIndex) {
  std::chrono::time_point<std::chrono::system_clock> start =
      std::chrono::system_clock::now();
  auto afi = afi::audio_file_index{self.parent_path() / "audiofileindex"};
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

TEST_F(AFI, LargeFileIndex) {
  auto afi = afi::audio_file_index{self.parent_path() / "NotActuallyFiles"};
  EXPECT_NE(afi.get_hash(), 0);
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
    mp3 += ext == ".mp3";
    m4a += ext == ".m4a";
    jpg += ext == ".jpg";
    flac += ext == ".flac";
    i++;
  });
  // There should be a total of 870 files in the test directory.
  EXPECT_EQ(mp3, 739);
  EXPECT_EQ(m4a, 3);
  EXPECT_EQ(jpg, 127);
  EXPECT_EQ(flac, 1);
  EXPECT_EQ(i, mp3 + m4a + jpg + flac);
}