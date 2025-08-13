#include <iostream>
#include <string>

#include <crow.h>

#include "CommonTypes.hpp"
#include "handlers.hpp"
#include "websocket.hpp"

namespace websocket {

crow::websocket::connection* webSocket = nullptr;

void configure(crow::SimpleApp& app) {
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
}

void keep_alive() {
  if (webSocket) {
    webSocket->send_text(
        std::to_string(static_cast<uint32_t>(Shared::SocketMsg::KeepAlive)) +
        ";");
  }
}

} // namespace websocket