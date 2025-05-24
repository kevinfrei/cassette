#include <filesystem>
#include <string>

namespace files {

void SetProgramLocation();
std::string PathToMimeType(const std::filesystem::path& filePath);
std::filesystem::path GetWebDir();

} // namespace files