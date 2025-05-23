#include <filesystem>
#include <string>

#include "files.h"
#include "handlers.h"
#include "quitting.h"

crow::response handle_file_path(const crow::request& req,
                                const std::string& path) {
  keep_alive();

  // std::cout << "Path: " << path << std::endl;
  crow::response resp;
  std::filesystem::path p = GetAppDir() / "www";
  p = p / (path.empty() ? "index.html" : path);
  resp.set_static_file_info(p.generic_string());
  resp.set_header("Content-type", FilePathToMimeType(p));
  return resp;
}

crow::response handle_tune(const crow::request& req, const std::string& path) {
  keep_alive();
  crow::response resp;
  std::filesystem::path p = std::filesystem::path{"/home/freik"};
  p = p / path;
  std::cout << "Tune: " << path << " to file " << p.generic_string()
            << std::endl;
  resp.set_static_file_info(p.generic_string());
  resp.set_header("Content-type", FilePathToMimeType(p));
  return resp;
}

crow::response handle_api(const crow::request& req, const std::string& path) {
  keep_alive();

  // std::cout << "API Path: " << path << std::endl;
  crow::response resp;
  resp.code = 200;
  resp.body = "{\"path\":\"" + path + "\"}";
  resp.set_header("Content-Type", "text/json");
  return resp;
}

crow::response handle_keepalive() {
  keep_alive();
  crow::response resp;
  resp.code = 200;
  resp.body = "OK";
  return resp;
}

crow::response handle_quit() {
  quit = true;
  return crow::response(200);
}