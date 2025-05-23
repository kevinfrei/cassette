#include <filesystem>
#include <string>

void SetProgramLocation();
std::string FilePathToMimeType(const std::filesystem::path& filePath);
std::filesystem::path GetWebDir();
