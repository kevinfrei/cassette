#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include <crow.h>
#include <gtest/gtest.h>

#include "config.h"
#include "files.h"

TEST(Config, TheBasics) {
  files::set_program_location();
  std::filesystem::path res = config::get_path();
  EXPECT_FALSE(res.empty());
  EXPECT_EQ(res.stem(), files::get_app_name());
  std::cout << "Config path: " << res.generic_string() << std::endl;
}