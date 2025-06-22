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

// Mac and Linux can't have colons or slashes in filenames.
// Windows can't have a whole range of characters in filenames,
// *plus* names can't *end* in a period or space.
// Windows also doesn't allow filenames to have control characters
// (And mac/linux shouldn't either, but they don't enforce it).
// In addition, Windows, Mac, and Linux all *may* have case-insensitive
// filesystems, so we need to encode filenames in a way that keeps all
// that in mind.
std::filesystem::path file_name_encode(std::string_view filename) {
  // Encode the string provided as a valid, unique filename, which can
  // be re-translated back to it's original value.
  std::ostringstream oss;
  // The idea is to replace invalid characters with _X values,
  // and encode a case-flip as _-, so that we can reconstruct
  // the original filename later.
  bool is_upper = true; // We start in uppercase mode
  for (char c : filename) {
    switch (c) {
      case '/':
        oss << "_s"; // Encode slashes
        break;
      case '\\':
        oss << "_b"; // Encode backslashes
        break;
      case ':':
        oss << "_c"; // Encode colons
        break;
      case '*':
        oss << "_a"; // Encode asterisks
        break;
      case '?':
        oss << "_q"; // Encode question marks
        break;
      case '"':
        oss << "_d"; // Encode double quotes
        break;
      case '<':
        oss << "_l"; // Encode less than
        break;
      case '>':
        oss << "_g"; // Encode greater than
        break;
      case '|':
        oss << "_p"; // Encode pipes
        break;
      case '.':
        oss << "_x"; // Encode periods/extensions separators
        break;
      case ' ':
        oss << "_e"; // Encode spaces
        break;
      case '_':
        oss << "__"; // Encode underscores as __ to avoid confusion with _U
        break;
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        oss << "_n"
            << static_cast<char>(c + '0'); // Encode values 0-9 as _n0 to _n9
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
      case 30:
      case 31:
        oss << "_n"
            << static_cast<char>(c + 'a' - 10); // values 10-31 as _na to _nu
        break;
      default:
        if (!isalpha(c))
          oss << c; // Keep non-alphabetic characters as is
        else {
          if (!!isupper(c) != is_upper) {
            oss << "_-";
            is_upper = !is_upper; // Toggle case mode
          }
          oss << static_cast<char>(tolower(c)); // Convert to lowercase
        }
    }
  }
  return oss.str();
}

std::optional<std::string> file_name_decode(std::string_view filename) {
  // Decode the encoded filename back to its original form.
  std::ostringstream oss;
  bool is_upper = true; // We start in uppercase mode
  bool post_underscore = false; // Flag to handle post-underscore case toggling
  bool control_char = false; // Flag to handle control characters
  for (char c : filename) {
    if (post_underscore) {
      // If we are in post-underscore mode, we expect a special character
      switch (tolower(c)) {
        case 's':
          oss << '/';
          break;
        case 'b':
          oss << '\\';
          break;
        case 'c':
          oss << ':';
          break;
        case 'a':
          oss << '*';
          break;
        case 'q':
          oss << '?';
          break;
        case 'd':
          oss << '"';
          break;
        case 'l':
          oss << '<';
          break;
        case 'g':
          oss << '>';
          break;
        case 'p':
          oss << '|';
          break;
        case '_':
          oss << '_';
          break;
        case 'x':
          oss << '.'; // Periods "extensions separators"
          break;
        case 'e':
          oss << ' '; // Spaces
          break;
        case '-':
          is_upper = !is_upper; // Toggle case mode
          break;
        case 'n':
          control_char = true; // Flag for control characters
          break;
        default:
          return std::nullopt; // Invalid character after underscore
      }
      post_underscore = false; // Reset the flag after processing
    } else if (c == '_') {
      post_underscore = true; // Set the flag for next character
    } else if (control_char) {
      // If we are in control character mode, we expect a digit
      if (std::isdigit(c)) {
        oss << static_cast<char>(c - '0'); // Convert to character
      } else if (std::isalpha(c)) {
        oss << static_cast<char>(tolower(c) - 'a' + 10); // Convert to character
      } else {
        return std::nullopt; // Invalid control character
      }
      control_char = false; // Reset the flag after processing
    } else if (!is_valid_url_char(c)) {
      return std::nullopt; // Invalid character
    } else if (!!isupper(c) != is_upper) {
      oss << "_-"; // Case flip
      is_upper = !is_upper; // Toggle case mode
    } else {
      oss << static_cast<char>(is_upper ? toupper(c)
                                        : tolower(c)); // Convert to lowercase
    }
  }
  if (post_underscore || control_char) {
    return std::nullopt; // Incomplete encoding
  } else {
    return oss.str();
  }
}

} // namespace tools