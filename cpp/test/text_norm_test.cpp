#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "text_normalization.hpp"

TEST(unicode, BasicNormalization) {
  // const char *input =
  // u8"ÀÁÂÃÄÅàáâãäåÈÉÊËèéêëÌÍÎÏìíîïÒÓÔÕÖØòóôõöøÙÚÛÜùúûüÇçÑñÝýÿ";
  const char* input = reinterpret_cast<const char*>(
      "ÀÁÂÃÄÅàáâãäåÈÉÊËèéêëÌÍÎÏìíîïÒÓÔÕÖØòóôõöøÙÚÛÜùúûüÇçÑñ");
  const char* expected = reinterpret_cast<const char*>(
      u8"A\u0300A\u0301A\u0302A\u0303A\u0308A\u030a"
      "a\u0300a\u0301a\u0302a\u0303a\u0308a\u030a"
      "E\u0300E\u0301E\u0302E\u0308e\u0300e\u0301e\u0302e\u0308"
      "I\u0300I\u0301I\u0302I\u0308i\u0300i\u0301i\u0302i\u0308"
      "O\u0300O\u0301O\u0302O\u0303O\u0308O\u00f8o\u0300o\u0301o\u0302o\u0303o"
      "\u0308o\u00f8"
      "U\u0300U\u0301U\u0302U\u0308u\u0300u\u0301u\u0302u\u0308"
      "C\u0327c\u0327N\u0303n\u0303Y\u0301y\u0301y");
  std::string normalized = txtnorm::normalize_latin_to_utf8(input);
  EXPECT_TRUE(normalized == expected);
}