#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <crow.h>

namespace tools {

void e404(crow::response& resp, const std::string& message);
std::optional<std::uint64_t> read_uint64_t(std::string_view sv);
std::optional<std::string> url_decode(std::string_view sv);

std::filesystem::path file_name_encode(std::string_view filename);
std::optional<std::string> file_name_decode(std::string_view filename);

} // namespace tools