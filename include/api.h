#include <string_view>

#include <crow/http_response.h>

namespace api {

// Functions to handle the HTTP requests (arg validation, etc...)
void read_from_storage(crow::response& resp, std::string_view data);
void write_to_storage(crow::response& resp, std::string_view data);
void delete_from_storage(crow::response& resp, std::string_view data);

} // namespace api