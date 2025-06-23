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

TEST(Config, Storage) {
  files::set_program_location();
  // Test if the item is writable
  EXPECT_TRUE(config::write_to_storage("test_key", "test-value"));
  EXPECT_TRUE(config::write_to_storage("Test key", "test-value2"));
  auto read_value = config::read_from_storage("test_key");
  EXPECT_TRUE(read_value.has_value());
  EXPECT_EQ(read_value.value(), "test-value");
  // Test if the item can be overwritten
  EXPECT_TRUE(config::write_to_storage("test_key", "new-value"));
  read_value = config::read_from_storage("test_key");
  // Let's flush the cache to ensure we read from *storage*
  config::flush_storage_cache();
  EXPECT_TRUE(read_value.has_value());
  EXPECT_EQ(read_value.value(), "new-value");
  // Test if the item can be deleted
  EXPECT_TRUE(config::delete_from_storage("test_key"));
  read_value = config::read_from_storage("test_key");
  EXPECT_FALSE(read_value.has_value());
  // Test that the item is not found for deletion
  EXPECT_FALSE(config::delete_from_storage("test_key"));
}