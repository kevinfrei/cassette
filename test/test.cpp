#include <crow.h>
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "json_pickling.hpp"

// Demonstrate some basic assertions.
TEST(Test, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(JsonPickling, uint32) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(123456);
  EXPECT_EQ(json_value.t(), crow::json::type::Number);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  std::optional<uint32_t> int_value = from_json<uint32_t>(json_value2);
  EXPECT_TRUE(int_value.has_value());
  EXPECT_EQ(int_value.value(), 123456);
  std::optional<bool> bool_value = from_json<bool>(json_value2);
  EXPECT_FALSE(bool_value.has_value());
}

TEST(JsonPickling, int32) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(-234567);
  EXPECT_EQ(json_value.t(), crow::json::type::Number);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto int_value = from_json<int32_t>(json_value2);
  EXPECT_TRUE(int_value.has_value());
  EXPECT_EQ(int_value.value(), -234567);
  std::optional<std::string> str_val = from_json<std::string>(json_value2);
  EXPECT_FALSE(str_val.has_value());
}

TEST(JsonPickling, uint16) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(12345);
  EXPECT_EQ(json_value.t(), crow::json::type::Number);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto int_value = from_json<uint16_t>(json_value2);
  EXPECT_TRUE(int_value.has_value());
  EXPECT_EQ(int_value.value(), 12345);
  std::optional<std::string> str_val = from_json<std::string>(json_value2);
  EXPECT_FALSE(str_val.has_value());
}

TEST(JsonPickling, int16) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(-1234);
  EXPECT_EQ(json_value.t(), crow::json::type::Number);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  std::optional<int16_t> int_value = from_json<int16_t>(json_value2);
  EXPECT_TRUE(int_value.has_value());
  EXPECT_EQ(int_value.value(), -1234);
  std::optional<std::string> str_val = from_json<std::string>(json_value2);
  EXPECT_FALSE(str_val.has_value());
}

TEST(JsonPickling, uint8) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(123);
  EXPECT_EQ(json_value.t(), crow::json::type::Number);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto int_value = from_json<uint8_t>(json_value2);
  EXPECT_TRUE(int_value.has_value());
  EXPECT_EQ(int_value.value(), 123);
  std::optional<std::string> str_val = from_json<std::string>(json_value2);
  EXPECT_FALSE(str_val.has_value());
}

TEST(JsonPickling, int8) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(-12);
  EXPECT_EQ(json_value.t(), crow::json::type::Number);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  std::optional<int8_t> int_value = from_json<int8_t>(json_value2);
  EXPECT_TRUE(int_value.has_value());
  EXPECT_EQ(int_value.value(), -12);
  std::optional<std::string> str_val = from_json<std::string>(json_value2);
  EXPECT_FALSE(str_val.has_value());
}

TEST(JsonPickling, true) {
  crow::json::wvalue json_value = to_json(true);
  EXPECT_EQ(json_value.t(), crow::json::type::True);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto bool_value = from_json<bool>(json_value2);
  EXPECT_TRUE(bool_value.has_value());
  EXPECT_EQ(bool_value.value(), true);
  auto int_value = from_json<uint32_t>(json_value2);
  EXPECT_FALSE(int_value.has_value());
}

TEST(JsonPickling, false) {
  crow::json::wvalue json_value = to_json(false);
  EXPECT_EQ(json_value.t(), crow::json::type::False);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto bool_value = from_json<bool>(json_value2);
  EXPECT_TRUE(bool_value.has_value());
  EXPECT_EQ(bool_value.value(), false);
  auto int_value = from_json<uint16_t>(json_value2);
  EXPECT_FALSE(int_value.has_value());
}

TEST(JsonPickling, double) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(1.25);
  EXPECT_EQ(json_value.t(), crow::json::type::Number);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto double_value = from_json<double>(json_value2);
  EXPECT_TRUE(double_value.has_value());
  EXPECT_EQ(double_value.value(), 1.25);
  auto int_value = from_json<uint32_t>(json_value2);
  EXPECT_FALSE(int_value.has_value());
}

TEST(JsonPickling, float) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(1.25f);
  EXPECT_EQ(json_value.t(), crow::json::type::Number);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto double_value = from_json<float>(json_value2);
  EXPECT_TRUE(double_value.has_value());
  EXPECT_EQ(double_value.value(), 1.25f);
  auto int_value = from_json<uint32_t>(json_value2);
  EXPECT_FALSE(int_value.has_value());
}

TEST(JsonPickling, c_string) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json("hello");
  EXPECT_EQ(json_value.t(), crow::json::type::String);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto str_value = from_json<std::string>(json_value2);
  EXPECT_TRUE(str_value.has_value());
  EXPECT_EQ(str_value.value(), "hello");
  auto int_value = from_json<uint32_t>(json_value2);
  EXPECT_FALSE(int_value.has_value());
}

TEST(JsonPickling, std_string) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json(std::string("hello"));
  EXPECT_EQ(json_value.t(), crow::json::type::String);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto str_value = from_json<std::string>(json_value2);
  EXPECT_TRUE(str_value.has_value());
  EXPECT_EQ(str_value.value(), "hello");
  auto int_value = from_json<uint32_t>(json_value2);
  EXPECT_FALSE(int_value.has_value());
}

TEST(JsonPickling, char) {
  // Test the conversion of basic types to and from JSON
  crow::json::wvalue json_value = to_json('a');
  EXPECT_EQ(json_value.t(), crow::json::type::String);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto str_value = from_json<char>(json_value2);
  EXPECT_TRUE(str_value.has_value());
  EXPECT_EQ(str_value.value(), 'a');
  auto int_value = from_json<uint32_t>(json_value2);
  EXPECT_FALSE(int_value.has_value());
}

TEST(JsonPickling, std_vector) {
  // Test the conversion of basic types to and from JSON
  std::vector<int> vec = {1, 2, 3};
  crow::json::wvalue json_value = to_json(vec);
  EXPECT_EQ(json_value.t(), crow::json::type::List);
  std::string s = json_value.dump();
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto vec_value = from_json<std::vector<int>>(json_value2);
  EXPECT_TRUE(vec_value.has_value());
  EXPECT_EQ(vec_value.value(), vec);
  auto int_value = from_json<uint32_t>(json_value2);
  EXPECT_FALSE(int_value.has_value());
  EXPECT_EQ(vec_value.value().size(), 3);
  EXPECT_EQ(vec_value.value()[0], 1);
  EXPECT_EQ(vec_value.value()[1], 2);
  EXPECT_EQ(vec_value.value()[2], 3);
}

TEST(JsonPickling, std_tuple) {
  // Test the conversion of basic types to and from JSON
  std::tuple<int, double, std::string> tup = {1, 2.5, "hello"};
  crow::json::wvalue json_value = to_json(tup);
  EXPECT_EQ(json_value.t(), crow::json::type::List);
  std::string s = json_value.dump();
  std::cout << "Tuple as JSON: " << s << std::endl;
  crow::json::rvalue json_value2 = crow::json::load(s);
  auto tup_value = from_json<std::tuple<int, double, std::string>>(json_value2);
  EXPECT_TRUE(tup_value.has_value());
  EXPECT_EQ(std::get<0>(tup_value.value()), 1);
  EXPECT_EQ(std::get<1>(tup_value.value()), 2.5);
  EXPECT_EQ(std::get<2>(tup_value.value()), "hello");
  auto tup2 = from_json<std::tuple<std::string, int, double, bool>>(
      crow::json::load("[\"hi\",15,4.125,false]"));
  EXPECT_TRUE(tup2.has_value());
  EXPECT_EQ(std::get<0>(tup2.value()), "hi");
  EXPECT_EQ(std::get<1>(tup2.value()), 15);
  EXPECT_EQ(std::get<2>(tup2.value()), 4.125);
  EXPECT_EQ(std::get<3>(tup2.value()), false);
}

// TODO: Test std::set, std::map, plus the 3 different enumeration
// types

TEST(JsonPickling, sets) {
  std::set<char> theSet{'a', 'c', 'd'};
  crow::json::wvalue json_value = to_json(theSet);
  EXPECT_EQ(json_value.t(), crow::json::type::Object);
}