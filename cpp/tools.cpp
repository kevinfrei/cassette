#include <iostream>
#include <string>

#include <crow.h>

void e404(crow::response& resp, const std::string& message) {
  std::cerr << "Error 404: " << message << std::endl;
  resp.code = 404;
  resp.body = message;
  resp.set_header("Content-Type", "text/plain");
}
