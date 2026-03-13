
#include <cstdint>
#include <iterator>
#include <random>
#include <string>

#include <crow.h>

#include "CommonTypes.hpp"
#include "api.hpp"
#include "config.hpp"
#include "files.hpp"
#include "handlers.hpp"
#include "json_pickling.hpp"
#include "quitting.hpp"
#include "websocket.hpp"
#include "window.hpp"

#include "setup.hpp"

namespace {

uint16_t port = 0;

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

crow::SimpleApp* theApp = nullptr;

std::thread* server_thread = nullptr;

void server_thread_func() {
  theApp->port(setup::get_random_port()).multithreaded().run();
}

} // namespace
namespace setup {

uint16_t get_random_port() {
  if (port == 0) {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 16383);
    port = 49152 + static_cast<uint16_t>(dist(rd)); // Use ports in the range
                                                    // 49152-65535
  }
  return port;
}

void init() {
  setlocale(LC_ALL, ".UTF8");
  files::set_program_location();
  std::string url = GetRootUrl();
  CROW_LOG_INFO << "Starting server at " << url;

  // Configure the server in a separate thread
  theApp = new crow::SimpleApp();
  theApp->loglevel(crow::LogLevel::Info);
  ConfigureRoutes(*theApp, url);
  server_thread = new std::thread(server_thread_func);
  server_thread->detach(); // Allow it to run independently
}

void run() {
  // Wait for a while
  // TODO: this is dumb: I should be able to detect when the server
  // is ready, but this is good enough for now.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // Block until the page is closed
  CROW_LOG_INFO << "*** Launching the browser:" << GetRootUrl();
  window::open(GetRootUrl());
  CROW_LOG_INFO << "******************** Shutting down server...";
  theApp->stop();
  if (server_thread->joinable()) {
    server_thread->join();
  }
  delete server_thread;
  delete theApp;
}
} // namespace setup
