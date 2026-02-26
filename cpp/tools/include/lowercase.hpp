#include <algorithm>
#include <cctype>
#include <cwctype>
#include <filesystem>
#include <type_traits>

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
void lowercase_extension(std::filesystem::path& p);
