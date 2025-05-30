#include <filesystem>
#include <iostream>

#include <boost/dll/runtime_symbol_info.hpp>

#include "SharedConstants.hpp"
#include "files.h"

namespace files {

std::filesystem::path program_location;

void SetProgramLocation() {
  program_location = boost::dll::program_location().string();
}

std::filesystem::path GetWebDir() {
  std::filesystem::path cur = program_location.parent_path().parent_path();
  return cur / "www";
}

std::string PathToMimeType(const std::filesystem::path& filePath) {
  const std::string extension = filePath.extension().generic_string();
  if (extension == ".txt") {
    return "text/plain";
  } else if (extension == ".css" || extension == ".scss") {
    return "text/css";
  } else if (extension == ".html" || extension == ".htm") {
    return "text/html";
  } else if (extension == ".js") {
    return "text/javascript";
  } else if (extension == ".gif") {
    return "image/gif";
  } else if (extension == ".jpg") {
    return "image/jpeg";
  } else if (extension == ".png") {
    return "image/png";
  } else if (extension == ".svg") {
    return "image/svg+xml";
  } else if (extension == ".flac") {
    return "audio/flac";
  } else if (extension == ".m4a") {
    return "audio/m4a";
  } else if (extension == ".mp3") {
    return "audio/mp3";
  }
  std::cout << "unknown: " << extension << std::endl;
  return "text/html";
}

#if 0
std::string LoadFileWithMimeType(const std::filesystem::path& path,
                                 std::string& mime_type) {
  std::ifstream input_file(path, std::ifstream::binary);

  // Get file size
  input_file.seekg(0, input_file.end);
  size_t file_size = input_file.tellg();
  input_file.seekg(0, input_file.beg);

  std::string ret;
  ret.resize(file_size);
  input_file.read(ret.data(), file_size);
  input_file.close();

  mime_type = ExtensionToMimeType(path.extension().string());

  return ret;
}
#endif

} // namespace files