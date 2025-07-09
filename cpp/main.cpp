#include <cstdint>
#include <iostream>
#include <string>

#include <crow.h>

// #include "MediaInfo/MediaInfo.h"

#include "files.h"
#include "handlers.h"
#include "quitting.h"
#include "setup.h"
#include "websocket.h"
#include "window.h"

std::string GetRootUrl() {
  return "http://localhost:" + std::to_string(get_random_port()) +
         "/www/index.html";
}

void ConfigureRoutes(crow::SimpleApp& app, const std::string& /*url*/) {
  // Define the routes:
  // Try a websocket route:
  websocket::configure(app);
  CROW_ROUTE(app, "/www/<path>")(handlers::file_path);
  CROW_ROUTE(app, "/api/<path>")(handlers::api);
  CROW_ROUTE(app, "/tune/<path>")(handlers::tune);
  CROW_ROUTE(app, "/keepalive")
      .methods(crow::HTTPMethod::GET,
               crow::HTTPMethod::POST,
               crow::HTTPMethod::PUT)(handlers::keepalive);
  CROW_ROUTE(app, "/quit")(handlers::quit);
}

int main(void) {
  crow::SimpleApp app;

  files::set_program_location();

  const uint16_t port = get_random_port();
  std::string url = GetRootUrl();
  ConfigureRoutes(app, url);
  window::open(url);
  auto _a = app.port(port).multithreaded().run_async();
  int i = 0;
  while (!quitting::should_quit()) {
    _a.wait_for(std::chrono::seconds(1));
    quitting::loop_wait();
    if (i++ % 5 == 0)
      websocket::keep_alive();
  }
  app.stop();
  return 0;
}
