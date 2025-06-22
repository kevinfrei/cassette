#include <iostream>
#include <string_view>

#include <crow/http_response.h>
#include <crow/json.h>

#include "api.h"
#include "config.h"
#include "tools.h"

namespace api {

std::string_view get_key(std::string_view data) {
  // the key is the first part of the data before an (optional) slash
  auto pos = data.find('/');
  if (pos != std::string_view::npos) {
    return data.substr(0, pos);
  }
  return data; // Return the whole data if no space found
}

void ReadFromStorage(crow::response& resp, std::string_view data) {
  auto key = get_key(data);
  std::cout << "Reading " << key << "  from storage: [" << data << "]"
            << std::endl;
  auto result = config::readFromStorage(key);
  if (!result) {
    resp.code = 204; // No Content
  } else {
    resp.code = 200; // OK
    resp.set_header("Content-Type", "application/json");
    resp.body = *result;
  }
}

void WriteToStorage(crow::response& resp, std::string_view data) {
  auto key = get_key(data);
  if (key.length() == data.length()) {
    // If the key is the same as the data, it means no slash was found
    std::cerr << "Invalid (missing) data for key " << key
              << " in storage request: [" << data << "]" << std::endl;
    return;
  }
  auto maybe_value = tools::url_decode(data.substr(key.length() + 1));
  if (!maybe_value) {
    std::cerr << "Invalid (malformed) value for key " << key
              << " in storage request: [" << data << "]" << std::endl;
    return;
  }
  auto json = crow::json::load(*maybe_value);
  std::cout << "Writing data " << json << " for " << key << " to storage"
            << std::endl;
  std::ostringstream os;
  os << json;
  if (!config::writeToStorage(key, os.str())) {
    resp.code = 500; // Internal Server Error
  } else {
    resp.code = 200; // OK
  }
}

void DeleteFromStorage(crow::response& resp, std::string_view data) {
  auto key = get_key(data);
  std::cout << "Deleting " << key << " from storage: [" << data << "]"
            << std::endl;
  if (!config::removeFromStorage(key)) {
    resp.code = 500; // Internal Server Error
  } else {
    resp.code = 200; // OK
  }
}

} // namespace api