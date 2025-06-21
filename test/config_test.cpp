#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include <crow.h>
#include <gtest/gtest.h>

#include "config.h"
#include "files.h"

TEST(Config, TheBasics) {
  files::SetProgramLocation();
  std::filesystem::path res = config::getPath();
  EXPECT_FALSE(res.empty());
  EXPECT_EQ(res.stem(), files::GetAppName());
  std::cout << "Config path: " << res.generic_string() << std::endl;
}