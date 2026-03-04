#include <algorithm>
#include <cctype>
#include <cwctype>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>

namespace text {

// Normalize a string to diacritic post-modifiers form. This is a very
// simplistic implementation that only handles a few common cases. A full
// implementation would use ICU or similar library.
std::string normalize_latin_to_utf8(std::string_view input);

// This is the most likely entrypoint for callers. It will take a string that
// may be in UTF-8 or Latin-1, and return a normalized UTF-8 string with
// diacritics as combining characters.
std::string normalize_utf8_or_latin(std::string_view input);
std::string normalize_utf8_or_latin(std::u8string_view input);

// Helper to handle both char and wchar_t lowering
template <typename T>
T toggle_lower(T c) {
  if constexpr (std::is_same_v<T, wchar_t>) {
    return std::towlower(c);
  } else {
    return static_cast<T>(std::tolower(static_cast<unsigned char>(c)));
  }
}

std::string lowercase(std::string_view str);
bool iequals(std::string_view lhs, std::string_view rhs);

} // namespace text