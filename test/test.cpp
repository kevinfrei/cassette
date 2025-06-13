#include <gtest/gtest.h>

#include "CommonTypes.hpp"

// Demonstrate some basic assertions.
TEST(Test, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(JsonPickling, BaseTypes) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(42);
  EXPECT_EQ(json_value.i(), 42);

  std::optional<int> int_value = from_json<int>(json_value);
  EXPECT_TRUE(int_value.has_value());
  EXPECT_EQ(int_value.value(), 42);

  json_value = to_json(1.25);
  EXPECT_EQ(json_value.d(), 1.25);

  std::optional<double> double_value = from_json<double>(json_value);
  EXPECT_TRUE(double_value.has_value());
  EXPECT_EQ(double_value.value(), 1.25);
}