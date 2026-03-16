#include <algorithm>
#include <cctype>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "text_tools.hpp"

#include "file_tools.hpp"

namespace fs = std::filesystem;

namespace files {

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
    lowered_ext += text::toggle_lower(c);
  }

  p.replace_extension(lowered_ext);
}

void foreach_line_in_file(const fs::path& filePath,
                          const std::function<void(const std::string&)>& fn) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filePath << "\n";
    return;
  }
  std::string line;
  while (std::getline(file, line)) {
    fn(line);
  }
}

bool is_hidden_file(const std::filesystem::path& path) {
  if (!std::filesystem::exists(path)) {
    return false; // File does not exist
  }

  // Check if the file is hidden by checking if it starts with a dot
  if (path.filename().string().starts_with('.')) {
    return true;
  }
  // On Windows, check the file attributes
#if defined(_WIN32)
  // On Windows, check if the file is hidden using the attributes
  DWORD attributes = GetFileAttributesW(path.c_str());
  if (attributes == INVALID_FILE_ATTRIBUTES) {
    return false; // File does not exist or error occurred
  }
  return (attributes & FILE_ATTRIBUTE_HIDDEN) != 0;
#else
  // TODO: Handle MacOS's extended file attributes?
  return false;
#endif
}

} // namespace files