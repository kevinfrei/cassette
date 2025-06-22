#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <crow.h>

namespace tools {

void e404(crow::response& resp, const std::string& message);
std::optional<std::uint64_t> read_uint64_t(std::string_view sv);
std::optional<std::string> url_decode(std::string_view sv);

} // namespace tools