#pragma once

#include <string_view>

#include <crow/http_response.h>

namespace api {

// Functions to handle the HTTP requests (arg validation, etc...)
void read_from_storage(crow::response& resp, std::string_view data);
void write_to_storage(crow::response& resp, std::string_view data);
void delete_from_storage(crow::response& resp, std::string_view data);

void get_playlists(crow::response& resp);
void load_playlist(crow::response& resp, std::string_view name);
void save_playlist(crow::response& resp, std::string_view name);

} // namespace api
