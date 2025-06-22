#include <filesystem>
#include <optional>
#include <string_view>

namespace config {

std::filesystem::path get_path();

bool write_to_storage(std::string_view key, std::string_view value);
std::optional<std::string> read_from_storage(std::string_view key);
bool delete_from_storage(std::string_view key);
void flush_storage_cache();
void clear_storage();

} // namespace config
