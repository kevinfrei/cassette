#include <filesystem>
#include <string>

namespace files {

void SetProgramLocation();
std::string PathToMimeType(const std::filesystem::path& filePath);
std::filesystem::path GetWebDir();
std::filesystem::path GetAppName();

} // namespace files