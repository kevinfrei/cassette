#include <string_view>

#include <crow/http_response.h>
#include <crow/json.h>

#include "api.hpp"
#include "config.hpp"
#include "json_pickling.hpp"
#include "playlists.hpp"
// #include "tools.hpp"

namespace api {

std::string_view get_key(std::string_view data) {
  // the key is the first part of the data before an (optional) slash
  auto pos = data.find('/');
  if (pos != std::string_view::npos) {
    return data.substr(0, pos);
  }
  return data; // Return the whole data if no space found
}

void read_from_storage(crow::response& resp, std::string_view data) {
  auto key = get_key(data);
  CROW_LOG_INFO << "Reading " << key << " from storage: <" << data << ">";
  auto result = config::read_from_storage(key);
  if (!result) {
    resp.code = 204; // No Content
  } else {
    resp.code = 200; // OK
    resp.set_header("Content-Type", "text/plain");
    resp.body = *result;
    CROW_LOG_INFO << "Read data for <" << key << "> from storage: <" << *result
                  << ">";
  }
}

void write_to_storage(crow::response& resp, std::string_view data) {
  auto key = get_key(data);
  if (key.length() == data.length()) {
    // If the key is the same as the data, it means no slash was found
    CROW_LOG_ERROR << "Invalid (missing) data for key " << key
                   << " in storage request: <" << data << ">";
    return;
  }
  auto value = data.substr(key.length() + 1);
  CROW_LOG_INFO << "Writing data " << value << " for " << key << " to storage";
  if (!config::write_to_storage(key, value)) {
    resp.code = 500; // Internal Server Error
  } else {
    resp.code = 200; // OK
  }
}

void delete_from_storage(crow::response& resp, std::string_view data) {
  auto key = get_key(data);
  CROW_LOG_INFO << "Deleting " << key << " from storage: <" << data << ">";
  if (!config::delete_from_storage(key)) {
    resp.code = 500; // Internal Server Error
  } else {
    resp.code = 200; // OK
  }
}

void get_playlists(crow::response& resp) {
  CROW_LOG_INFO << "Getting playlists from storage";
  auto result = playlist::get_names();
  if (!result) {
    resp.code = 204; // No Content
  } else {
    resp.code = 200; // OK
    resp.set_header("Content-Type", "application/json");
    resp.body = to_json(*result).dump();
    CROW_LOG_INFO << "Got playlists from storage: <" << resp.body << ">";
  }
}

void load_playlist(crow::response& resp, std::string_view name) {
  CROW_LOG_INFO << "Loading playlist " << name;
  auto result = playlist::load(name);
  if (!result) {
    resp.code = 204; // No Content
  } else {
    resp.code = 200; // OK
    resp.set_header("Content-Type", "application/json");
    resp.body = to_json(*result).dump();
    CROW_LOG_INFO << "Loaded playlist " << name << " from storage: <"
                  << resp.body << ">";
  }
}

void save_playlist(crow::response& resp, std::string_view name_and_keys) {
  CROW_LOG_INFO << "TODO: Saving playlist " << name_and_keys;
  resp.code = 200; // OK
}

} // namespace api