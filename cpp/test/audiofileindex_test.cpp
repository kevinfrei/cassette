#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include "CommonTypes.hpp"
#include "audiofileindex.h"

std::filesystem::path self{__FILE__};

TEST(AFI, Basics) {
  auto afi = afi::audio_file_index{self.parent_path() / "__tests__" /
                                   "audiofileindex"};
  EXPECT_NE(afi.get_hash(), 0);
  auto p = afi.get_location();
  // std::cout << p.generic_string() << std::endl;
  EXPECT_FALSE(p.generic_string().ends_with("."));
  int i = 0;
  afi.foreach_audio_file([&](const std::filesystem::path& path) {
    std::cout << "Audio file: " << path.generic_string() << std::endl;
    EXPECT_TRUE(path.is_absolute());
    EXPECT_TRUE(path.has_filename());
    EXPECT_TRUE(path.has_extension());
    EXPECT_TRUE(path.extension().generic_string().starts_with("."));
    i++;
  });
  EXPECT_EQ(i, 6);
}