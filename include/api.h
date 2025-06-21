#include <string_view>

#include <crow/http_response.h>

namespace api {

// Functions to handle the HTTP requests (arg validation, etc...)
void ReadFromStorage(crow::response& resp, std::string_view data);
void WriteToStorage(crow::response& resp, std::string_view data);
void DeleteFromStorage(crow::response& resp, std::string_view data);

} // namespace api