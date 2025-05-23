#include <iostream>
#include <random>
#include <string>

#if defined(_WIN32)
#define _WIN32_WINNT 0xA00
#endif

// #include "MediaInfo/MediaInfo.h"

#include <crow/app.h>

#include "handlers.h"
#include "quitting.h"
#include "window.h"

int GetRandomPort() {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, 16383);
  return 49152 + dist(rd);
}

std::string GetRootUrl(int port) {
  return "http://localhost:" + std::to_string(port) + "/www/index.html";
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
