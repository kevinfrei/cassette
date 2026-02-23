#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

#include <crow.h>

#include "files.hpp"
#include "handlers.hpp"
#include "quitting.hpp"
#include "setup.hpp"
#include "websocket.hpp"
#include "window.hpp"

std::string GetRootUrl() {
  return "http://localhost:" + std::to_string(setup::get_random_port()) +
         "/www/index.html";
}

void ConfigureRoutes(crow::SimpleApp& app, const std::string& /*url*/) {
  // Define the routes:
  // Try a websocket route:
  websocket::configure(app);
  CROW_ROUTE(app, "/www/<path>")(handlers::www_path);
  CROW_ROUTE(app, "/api/<path>")(handlers::api);
  CROW_ROUTE(app, "/tune/<path>")(handlers::tune);
  CROW_ROUTE(app, "/images/<path>")(handlers::images);
  CROW_ROUTE(app, "/keepalive")
      .methods(crow::HTTPMethod::GET,
               crow::HTTPMethod::POST,
               crow::HTTPMethod::PUT)(handlers::keepalive);
  CROW_ROUTE(app, "/quit")(handlers::quit);
}

int main(void) {
  setlocale(LC_ALL, ".UTF8");

  crow::SimpleApp app;
  app.loglevel(crow::LogLevel::Warning);
  files::set_program_location();

  const uint16_t port = setup::get_random_port();
  std::string url = GetRootUrl();
  std::cerr << "Starting server at " << url << std::endl;
  ConfigureRoutes(app, url);
  std::thread server_thread(
      [&app, port]() { app.port(port).multithreaded().run(); });
  server_thread.detach(); // Allow it to run independently
  // Wait for a while (this is dumb: I should be able to detect when the server
  // is ready, but this is good enough for now)
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // Block until the page is closed
  std::cerr << "******************** Launching the browser..." << std::endl;
  window::open(url);
  std::cerr << "******************** Shutting down server..." << std::endl;
  app.stop();
  if (server_thread.joinable()) {
    server_thread.join();
  }
  return 0;
}
