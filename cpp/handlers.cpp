#include <filesystem>
#include <string>

#include "CommonTypes.hpp"
#include "files.h"
#include "handlers.h"
#include "quitting.h"

namespace handlers {

crow::response file_path(const crow::request& req, const std::string& path) {
  quitting::keep_alive();

  // std::cout << "Path: " << path << std::endl;
  crow::response resp;
  std::filesystem::path p =
      files::GetWebDir() / (path.empty() ? "index.html" : path);
  resp.set_static_file_info_unsafe(p.generic_string());
  resp.set_header("Content-type", files::PathToMimeType(p));
  return resp;
}

crow::response tune(const crow::request& req, const std::string& path) {
  quitting::keep_alive();
  crow::response resp;
  std::filesystem::path p = std::filesystem::path{"/home/freik"};
  p = p / path;
  std::cout << "Tune: " << path << " to file " << p.generic_string()
            << std::endl;
  resp.set_static_file_info(p.generic_string());
  resp.set_header("Content-type", files::PathToMimeType(p));
  return resp;
}

crow::response api(const crow::request& req, const std::string& path) {
  quitting::keep_alive();

  // std::cout << "API Path: " << path << std::endl;
  crow::response resp;
  resp.code = 200;
  resp.body = "{\"path\":\"" + path + "\"}";
  resp.set_header("Content-Type", "text/json");
  return resp;
}

crow::response keepalive() {
  quitting::keep_alive();
  crow::response resp;
  resp.code = 200;
  resp.body = "OK";
  resp.set_header("Content-Type", "text/plain");
  return resp;
}

crow::response quit() {
  quitting::really_quit();
  return crow::response(200);
}

} // namespace handlers