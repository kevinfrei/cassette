#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include "CommonTypes.hpp"
#include "audiofileindex.h"

std::filesystem::path self{__FILE__};

TEST(AFI, SmallFileIndex) {
  auto afi = afi::audio_file_index{self.parent_path() / "__tests__" /
                                   "audiofileindex"};
  EXPECT_NE(afi.get_hash(), 0);
  auto p = afi.get_location();
  // std::cout << p.generic_string() << std::endl;
  EXPECT_FALSE(p.generic_string().ends_with("."));
  int i = 0;
  afi.foreach_audio_file([&](const std::filesystem::path& path) {
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
}

TEST(AFI, LargeFileIndex) {
  auto afi = afi::audio_file_index{self.parent_path() / "__tests__" /
                                   "NotActuallyFiles"};
  EXPECT_NE(afi.get_hash(), 0);
  auto p = afi.get_location();
  int i = 0;
  int mp3 = 0;
  int m4a = 0;
  int jpg = 0;
  int flac = 0;
  afi.foreach_audio_file([&](const std::filesystem::path& path) {
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