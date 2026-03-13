#include <filesystem>
#include <string>

#include <crow.h>
#include <gtest/gtest.h>

#include "files.hpp"

#include "images.hpp"

TEST(Images, JustTheOne) {
  files::set_program_location();
  auto path1 = image::get_image_path("somethin");
  EXPECT_TRUE(path1.generic_string().ends_with("/img/icon.svg"));
  auto path2 = image::get_image_path("somethin/else");
  EXPECT_TRUE(path2.generic_string().ends_with("/img/somethin.svg"));
}
