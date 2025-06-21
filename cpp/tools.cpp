#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include <crow.h>

void e404(crow::response& resp, const std::string& message) {
  std::cerr << "Error 404: " << message << std::endl;
  resp.code = 404;
  resp.body = message;
  resp.set_header("Content-Type", "text/plain");
}

std::optional<std::uint64_t> read_uint64_t(std::string_view sv) {
  uint64_t value = 0;
  for (auto chr : sv) {
    if (chr < '0' || chr > '9') {
      return std::nullopt;
    }
    value = value * 10 + (chr - '0');
  }
  return value;
}