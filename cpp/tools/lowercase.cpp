#include <algorithm>
#include <cctype>
#include <cwctype>
#include <filesystem>
#include <string>
#include <string_view>

#include "lowercase.hpp"

namespace fs = std::filesystem;

void lowercase_extension(fs::path& p) {
  if (!p.has_extension())
    return;

  // Use the native string type (std::string or std::wstring)
  auto native_ext = p.extension().native();
  std::basic_string_view<fs::path::value_type> view(native_ext);

  // 1. Check if any characters actually need lowering
  bool needs_change = std::any_of(view.begin(), view.end(), [](auto c) {
    if constexpr (std::is_same_v<decltype(c), wchar_t>) {
      return std::iswupper(c);
    } else {
      return std::isupper(static_cast<unsigned char>(c));
    }
  });

  if (!needs_change)
    return;

  // 2. Perform transformation using the native character type
  std::basic_string<fs::path::value_type> lowered_ext;
  lowered_ext.reserve(view.size());

  for (auto c : view) {
    lowered_ext += toggle_lower(c);
  }

  p.replace_extension(lowered_ext);
}

std::string lowercase(std::string_view str) {
  std::string result;
  result.reserve(str.size());
  for (char c : str) {
    result += toggle_lower(c);
  }
  return result;
}