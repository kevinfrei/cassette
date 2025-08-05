#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "text_normalization.hpp"

// clang-format off
constexpr uint8_t latin_chars[] = {
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, // upper A
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, // lower a
  0xc8, 0xc9, 0xca, 0xcb, 0xe8, 0xe9, 0xea, 0xeb, // E's
  0xcc, 0xcd, 0xce, 0xcf, 0xec, 0xed, 0xee, 0xef, // I's
  0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd8, // upper O
  0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf8, // lower O
  0xd9, 0xda, 0xdb, 0xdc, 0xf9, 0xfa, 0xfb, 0xfc, // U's
  0xc7, 0xe7, 0xd1, 0xf1, 0 // C's, N's
};
// clang-format on

TEST(unicode, BasicNormalization) {
  // const char *input =
  // u8"ÀÁÂÃÄÅàáâãäåÈÉÊËèéêëÌÍÎÏìíîïÒÓÔÕÖØòóôõöøÙÚÛÜùúûüÇçÑñÝýÿ";
  const char* input = reinterpret_cast<const char*>(&latin_chars[0]);
  // "ÀÁÂÃÄÅàáâãäåÈÉÊËèéêëÌÍÎÏìíîïÒÓÔÕÖØòóôõöøÙÚÛÜùúûüÇçÑñ");
  const char* expected = reinterpret_cast<const char*>(
      u8"A\u0300A\u0301A\u0302A\u0303A\u0308A\u030a"
      u8"a\u0300a\u0301a\u0302a\u0303a\u0308a\u030a"
      u8"E\u0300E\u0301E\u0302E\u0308e\u0300e\u0301e\u0302e\u0308"
      u8"I\u0300I\u0301I\u0302I\u0308i\u0300i\u0301i\u0302i\u0308"
      u8"O\u0300O\u0301O\u0302O\u0303O\u0308O\u0337"
      u8"o\u0300o\u0301o\u0302o\u0303o\u0308o\u0337"
      u8"U\u0300U\u0301U\u0302U\u0308u\u0300u\u0301u\u0302u\u0308"
      u8"C\u0327c\u0327N\u0303n\u0303"); // Y\u0301y\u0301 ");
  std::string normalized = txtnorm::normalize_latin_to_utf8(input);
  for (size_t i = 0; i < normalized.size(); i++) {
    if (normalized[i] != expected[i]) {
      std::cout << "Mismatch at " << i << ": expected '" << expected[i]
                << "' (0x" << std::hex << static_cast<int>(expected[i])
                << "), got '" << normalized[i] << "' (0x" << std::hex
                << static_cast<int>(normalized[i]) << ")\n";
    }
    EXPECT_EQ(normalized[i], expected[i]);
  }
  EXPECT_TRUE(normalized == expected);
}