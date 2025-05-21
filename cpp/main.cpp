#include <atomic>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>

#if defined(_WIN32)
#define _WIN32_WINNT 0xA00
#endif

// #include "MediaInfo/MediaInfo.h"

#include <crow/app.h>

#include "window.h"

std::string ExtensionToMimeType(const std::string& extension) {
  if (extension == ".txt") {
    return "text/plain";
  } else if (extension == ".css" || extension == ".scss") {
    return "text/css";
  } else if (extension == ".html" || extension == ".htm") {
    return "text/html";
  } else if (extension == ".js") {
    return "text/javascript";
  } else if (extension == ".gif") {
    return "image/gif";
  } else if (extension == ".jpg") {
    return "image/jpeg";
  } else if (extension == ".png") {
    return "image/png";
  } else if (extension == ".svg") {
    return "image/svg+xml";
  } else if (extension == ".flac") {
    return "audio/flac";
  } else if (extension == ".m4a") {
    return "audio/m4a";
  } else if (extension == ".mp3") {
    return "audio/mp3";
  }
  std::cout << "unknown: " << extension << std::endl;
  return "text/html";
}

#if 0
std::string LoadFileWithMimeType(const std::filesystem::path& path,
                                 std::string& mime_type) {
  std::ifstream input_file(path, std::ifstream::binary);

  // Get file size
  input_file.seekg(0, input_file.end);
  size_t file_size = input_file.tellg();
  input_file.seekg(0, input_file.beg);

  std::string ret;
  ret.resize(file_size);
  input_file.read(ret.data(), file_size);
  input_file.close();

  mime_type = ExtensionToMimeType(path.extension().string());

  return ret;
}
#endif

std::atomic_bool quit = false;
std::atomic_int8_t quit_timer = 10;

void keep_alive() {
  quit_timer.store(10);
}

int GetRandomPort() {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, 16383);
  return 49152 + dist(rd);
}

std::string GetRootUrl(int port) {
  return "http://localhost:" + std::to_string(port) + "/www/index.html";
}

crow::response handle_file_path(const crow::request& req,
                                const std::string& path) {
  keep_alive();

  // std::cout << "Path: " << path << std::endl;
  crow::response resp;
  std::filesystem::path p = std::filesystem::path{"www"};
  p = p / (path.empty() ? "index.html" : path);
  resp.set_static_file_info(p.generic_string());
  resp.set_header("Content-type",
                  ExtensionToMimeType(p.extension().generic_string()));
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
  resp.set_header("Content-type",
                  ExtensionToMimeType(p.extension().generic_string()));
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

int main(void) {
  crow::SimpleApp app;

  const int port = GetRandomPort();
  std::string url = GetRootUrl(port);

  // Define the routes:
  CROW_ROUTE(app, "/www/<path>")(handle_file_path);
  CROW_ROUTE(app, "/api/<path>")(handle_api);
  CROW_ROUTE(app, "/tune/<path>")(handle_tune);
  CROW_ROUTE(app, "/keepalive")
      .methods(crow::HTTPMethod::GET,
               crow::HTTPMethod::POST,
               crow::HTTPMethod::PUT)([]() {
        keep_alive();
        crow::response resp;
        resp.code = 200;
        resp.body = "OK";
        return resp;
      });
  CROW_ROUTE(app, "/quit")
  ([&](const crow::request& req) {
    quit = true;
    return crow::response(200);
  });

  window::open(url);
  auto _a = app.port(port).multithreaded().run_async();
  while (!quit) {
    _a.wait_for(std::chrono::seconds(1));
    if (quit_timer > 0) {
      quit_timer--;
    } else {
      quit = true;
    }
  }
  app.stop();
  return 0;
}
