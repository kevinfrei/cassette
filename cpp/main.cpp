#include <atomic>
#if defined(_WIN32)
#define _WIN32_WINNT 0xA00
#endif

#include "crow.h"

#include <filesystem>
#include <iostream>
#include <random>
#include <string>

std::string ExtensionToMimeType(const std::string& extension) {
  if (extension == "txt") {
    return "text/plain";
  } else if (extension == "css" || extension == "scss") {
    return "text/css";
  } else if (extension == "html" || extension == "htm") {
    return "text/html";
  } else if (extension == "js") {
    return "text/javascript";
  } else if (extension == "gif") {
    return "image/gif";
  } else if (extension == "jpg") {
    return "image/jpeg";
  } else if (extension == "png") {
    return "image/png";
  } else if (extension == "svg") {
    return "image/svg";
  } else if (extension == "flac") {
    return "audio/flac";
  } else if (extension == "m4a") {
    return "audio/m4a";
  } else if (extension == "mp3") {
    return "audio/mp3";
  }
  return "text/html";
}

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

void OpenMac(const std::string& url) {
  std::string target =
      "/Applications/Google\\ Chrome.app/Contents/MacOS/Google\\ Chrome "
      "--new-window --app=" +
      url;
  system(target.c_str());
}

std::atomic_bool quit = false;
std::atomic_int8_t quit_timer = 10;

void keep_alive() {
  quit_timer.store(10);
}

int main(void) {
  crow::SimpleApp app;
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, 16383);
  const int port = 49152 + dist(rd);
  std::string url =
      "http://localhost:" + std::to_string(port) + "/www/index.html";
  // Define a route
  CROW_ROUTE(app, "/www/<path>")
  ([&](const crow::request& req, const std::string& path) {
    keep_alive();
    std::cout << "Path: " << path << std::endl;
    crow::response resp;
    std::filesystem::path p = std::filesystem::path{"www"};
    if (path.empty()) {
      p = p / "index.html";
    } else {
      p = p / path;
    }
    // std::string mime_type = ExtensionToMimeType(p.extension().string());
    resp.set_static_file_info(p.generic_string());
    // resp = LoadFileWithMimeType(p / path, mime_type);
    // resp.set_header("Content-Type", mime_type);
    // resp.end();
    return resp;
  });

  CROW_ROUTE(app, "/api/<path>")
  ([&](const crow::request& req, const std::string& path) {
    keep_alive();
    std::cout << "API Path: " << path << std::endl;
    crow::response resp;
    resp.code = 200;
    resp.body = "{\"path\":\"" + path + "\"}";
    resp.set_header("Content-Type", "text/json");
    return resp;
  });
  CROW_ROUTE(app, "/keepalive")
  ([]() {
    keep_alive();
    std::cout << "Keepalive" << std::endl;
    crow::response resp;
    resp.code = 200;
    resp.body = "OK";
    return resp;
  });
  CROW_ROUTE(app, "/quit")
  ([&](const crow::request& req) {
    std::cout << "Quit" << std::endl;
    quit.store(true);
    return crow::response(200);
  });
  std::cout << url << std::endl;

#if defined(_WIN32)
  system(append("start ", url).c_str());
#elif defined(__APPLE__)
  OpenMac(url); // system("open http://localhost:" << port << "/index.html");
#elif defined(__linux__)
  system((std::string{"xdg-open "} + url)).c_str());
#else
  std::cout << "Please open " << url << " in your browser." << std::endl;
#endif
  auto _a = app.port(port).multithreaded().run_async();
  while (!quit.load()) {
    _a.wait_for(std::chrono::seconds(1));
    if (quit_timer > 0) {
      quit_timer--;
      std::cout << "Quit timer decremented: " << quit_timer << std::endl;
    } else {
      std::cout << "Quitting..." << std::endl;
      quit.store(true);
    }
    std::cout << "" << port << std::endl;
  }
  app.stop();
  return 0;
}
