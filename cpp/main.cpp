#include <iostream>
#include <random>
#include <string>

#include <crow/app.h>

// #include "MediaInfo/MediaInfo.h"

#include "files.h"
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

  SetProgramLocation();

  const int port = GetRandomPort();
  std::string url = GetRootUrl(port);

  // Define the routes:
  CROW_ROUTE(app, "/www/<path>")(handlers::file_path);
  CROW_ROUTE(app, "/api/<path>")(handlers::api);
  CROW_ROUTE(app, "/tune/<path>")(handlers::tune);
  CROW_ROUTE(app, "/keepalive")
      .methods(crow::HTTPMethod::GET,
               crow::HTTPMethod::POST,
               crow::HTTPMethod::PUT)(handlers::keepalive);
  CROW_ROUTE(app, "/quit")(handlers::quit);

  window::open(url);
  auto _a = app.port(port).multithreaded().run_async();
  while (!should_quit()) {
    _a.wait_for(std::chrono::seconds(1));
    loop_wait();
  }
  app.stop();
  return 0;
}
