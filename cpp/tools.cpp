#include <cctype>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include <crow.h>

namespace tools {

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

std::string url_encode(std::string_view value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (auto i = value.begin(), n = value.end(); i != n; ++i) {
    std::string_view::value_type c = (*i);

    // Keep alphanumeric and other accepted characters intact
    if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << int((unsigned char)c);
    escaped << std::nouppercase;
  }

  return escaped.str();
}
bool is_valid_url_char(char c) {
  // Check if the character is a valid URL character
  if (std::isalnum(c))
    return true;
  switch (c) {
    case '$':
    case '-':
    case '_':
    case '.':
    case '+':
    case '!':
    case '*':
    case '\'':
    case '(':
    case ')':
      return true;
    default:
      return false;
  }
}

int hex_val(char c) {
  assert(std::isxdigit(c) && "hex_val called with non-hex character");
  // Convert a hex character to its integer value
  if (std::isdigit(c)) {
    return c - '0'; // '0' to '9'
  } else {
    return std::toupper(c) - 'A' + 10; // 'A' to 'F'
  }
}

std::optional<std::string> url_decode(std::string_view value) {
  std::ostringstream unescaped;
  int hexPos = -1;
  int hexValue = 0;
  for (auto c : value) {
    if (hexPos == 0 || hexPos == 1) {
      // We are in a hex sequence
      if (std::isxdigit(c)) {
        hexValue = (hexValue << 4) | hex_val(c);
        if (hexPos == 1) {
          // We have both hex digits, convert to character}
          unescaped << static_cast<char>(hexValue);
          hexPos = -1; // Reset hex position
        } else {
          hexPos = 1; // Move to next hex digit
        }
      } else {
        return std::nullopt; // Invalid hex sequence
      }
    } else if (c == '%') {
      // Start of a hex sequence
      hexPos = 0;
    } else if (is_valid_url_char(c)) {
      // Normal character, just append it
      unescaped << c;
    } else {
      return std::nullopt; // Invalid character
    }
  }
  return unescaped.str();
}

} // namespace tools