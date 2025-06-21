#include <iostream>
#include <string_view>

#include <crow/http_response.h>

#include "api.h"

namespace api {

void ReadFromStorage(crow::response& resp, std::string_view data) {
  std::cout << "Reading from storage: " << data << std::endl;
}

void WriteToStorage(crow::response& resp, std::string_view data) {
  std::cout << "Writing from storage: " << data << std::endl;
}

void DeleteFromStorage(crow::response& resp, std::string_view data) {
  std::cout << "Deleting from storage: " << data << std::endl;
}

} // namespace api