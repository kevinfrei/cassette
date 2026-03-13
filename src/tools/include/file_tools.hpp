#pragma once

#include <filesystem>
#include <functional>
#include <string>
namespace files {

bool is_hidden_file(const std::filesystem::path& path);
void lowercase_extension(std::filesystem::path& p);
void foreach_line_in_file(const std::filesystem::path& filePath,
                          const std::function<void(const std::string&)>& fn);

} // namespace files
