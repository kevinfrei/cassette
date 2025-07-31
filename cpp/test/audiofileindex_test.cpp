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
  EXPECT_NE(afi.getHash(), 0);
  auto p = afi.getLocation();
  // std::cout << p.generic_string() << std::endl;
  EXPECT_FALSE(p.generic_string().ends_with("."));
}