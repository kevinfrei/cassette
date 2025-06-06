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
  return "http://localhost:" + std::to_string(GetRandomPort()) +
         "/www/index.html";
}

int main(void) {
  crow::SimpleApp app;

  files::SetProgramLocation();

  const int port = GetRandomPort();
  std::string url = GetRootUrl();
  crow::websocket::connection* theConn = nullptr;
  // Define the routes:

  // Try a websocket route:
  CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection& conn) -> void {
        std::cout << "WebSocket connection opened from " << conn.get_remote_ip()
                  << std::endl;
        theConn = &conn;
      })
      .onmessage([&](crow::websocket::connection& conn,
                     const std::string& data,
                     bool is_binary) -> void {
        std::cout << "Received WebSocket message: "
                  << (is_binary ? "Binary " : "Text ") << data.size()
                  << (is_binary ? " <not displaying> " : data) << " bytes from "
                  << conn.get_remote_ip() << std::endl;
      })
      .onclose([&](crow::websocket::connection& conn,
                   const std::string& reason /*, uint16_t code*/) {
        std::cout << "WebSocket connection closed from " << conn.get_remote_ip()
                  << " with reason: "
                  << reason /* << " and code " << code */ << std::endl;
        theConn = nullptr;
      });
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
  int i = 0;
  while (!quitting::should_quit()) {
    _a.wait_for(std::chrono::seconds(1));
    quitting::loop_wait();
    if (theConn) {
      // Send a keepalive message every 5 seconds
      theConn->send_text("keep alive the other way: " + std::to_string(i++));
    }
  }
  app.stop();
  return 0;
}
