#if defined(_WIN32)
#include <windows.h>
#endif

#include <filesystem>

#include "toolbox.h"

namespace toolbox {

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

} // namespace toolbox