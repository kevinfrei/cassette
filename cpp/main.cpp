#include <cstdint>
#include <iostream>
#include <string>

#include <crow.h>

// #include "MediaInfo/MediaInfo.h"

#include "files.h"
#include "handlers.h"
#include "quitting.h"
#include "setup.h"
#include "window.h"

std::string GetRootUrl() {
  return "http://localhost:" + std::to_string(get_random_port()) +
         "/www/index.html";
}

crow::websocket::connection* webSocket = nullptr;

void ConfigureRoutes(crow::SimpleApp& app, const std::string& url) {
  // Define the routes:
  // Try a websocket route:
  CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) -> void {
        std::cout << "WebSocket connection opened from " << conn.get_remote_ip()
                  << std::endl;
        webSocket = &conn;
      })
      .onmessage(handlers::socket_message)
      .onclose([&](crow::websocket::connection& conn,
                   const std::string& reason /*, uint16_t code*/) {
        std::cout << "WebSocket connection closed from " << conn.get_remote_ip()
                  << " with reason: "
                  << reason /* << " and code " << code */ << std::endl;
        webSocket = nullptr;
      });
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

  const int port = get_random_port();
  std::string url = GetRootUrl();
  ConfigureRoutes(app, url);
  window::open(url);
  auto _a = app.port(port).multithreaded().run_async();
  int i = 0;
  while (!quitting::should_quit()) {
    _a.wait_for(std::chrono::seconds(1));
    quitting::loop_wait();
    if (webSocket) {
      // Send a keepalive message every 5 seconds
      if (i++ % 5 == 0)
        webSocket->send_text("keep alive the other way: " + std::to_string(i));
    }
  }
  app.stop();
  return 0;
}
