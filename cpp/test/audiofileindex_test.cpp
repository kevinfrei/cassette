#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "CommonTypes.hpp"
#include "audiofileindex.h"

TEST(AFI, Basics) {
  EXPECT_TRUE(afi::audio_file_index::create("."));
}