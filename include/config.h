#include <filesystem>
#include <optional>
#include <string_view>

namespace config {

std::filesystem::path getPath();

bool writeToStorage(std::string_view key, std::string_view value);
std::optional<std::string> readFromStorage(std::string_view key);
bool removeFromStorage(std::string_view key);

} // namespace config
