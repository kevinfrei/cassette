#include <filesystem>
#include <iostream>
#include <string>

#include <crow.h>

#include "CommonTypes.hpp"
#include "api.h"
#include "files.h"
#include "handlers.h"
#include "quitting.h"
#include "setup.h"
#include "tools.h"
#include "tunes.h"

namespace handlers {

crow::response file_path(const crow::request& req, const std::string& path) {
  quitting::keep_alive();

  std::cout << "Path: " << path << std::endl;
  crow::response resp;
  std::filesystem::path p =
      files::GetWebDir() / (path.empty() ? "index.html" : path);
  if (p.filename() == "index.html") {
    // We need to process the index.html file to replace the websocket URL
    // with the correct one.

    // Read the contents of the index.html file,
    // Replace "window.wsport = 42;" with the actual port number
    std::ifstream file(p);
    if (!file.is_open()) {
      tools::e404(resp, "index.html not found");
      return resp;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    // Replace the placeholder with the actual port number
    std::string wsport = std::to_string(GetRandomPort());
    size_t pos = content.find("window.wsport = 42;");
    if (pos != std::string::npos) {
      content.replace(pos, 20, "window.wsport = " + wsport + ";");
      resp.body = content;
      resp.code = 200;
      resp.set_header("Content-Type", "text/html");
    } else {
      std::cerr << "Placeholder not found in index.html file: "
                << p.generic_string() << std::endl;
      resp.code = 500;
      resp.body = "Internal Server Error";
      resp.set_header("Content-Type", "text/plain");
    }
  } else {
    resp.set_static_file_info_unsafe(p.generic_string());
    resp.set_header("Content-type", files::PathToMimeType(p));
  }
  return resp;
}

crow::response tune(const crow::request& req, const std::string& path) {
  quitting::keep_alive();
  crow::response resp;
  auto maybe_song = get_tune(path);
  if (!maybe_song) {
    tools::e404(resp, "Tune not found");
    return resp;
  }
  const auto& song = maybe_song.value();
  resp.set_static_file_info_unsafe(song.generic_string());
  resp.set_header("Content-type", files::PathToMimeType(song));
  return resp;
}

crow::response api(const crow::request& req, const std::string& path) {
  quitting::keep_alive();

  std::cout << "API Path: " << path << std::endl;
  crow::response resp;
  size_t slashPos = path.find('/');
  slashPos = (slashPos == path.npos) ? path.size() : slashPos;
  auto maybeNum =
      tools::read_uint64_t(std::string_view{path.c_str(), slashPos});
  if (!maybeNum) {
    tools::e404(resp, "Invalid API arguments");
    return resp;
  }
  Shared::IpcCall call = static_cast<Shared::IpcCall>(*maybeNum);
  if (!Shared::is_valid(call)) {
    tools::e404(resp, "Unknown API");
    return resp;
  }
  auto ValidateAndCall =
      [&](std::function<void(crow::response&, std::string_view)> handle_call)
      -> void {
    if (slashPos == path.size()) {
      tools::e404(resp, "No data provided for API");
    } else {
      resp.code = 200;
      handle_call(
          resp,
          std::string_view{path.c_str() + slashPos, path.size() - slashPos});
    }
  };
  switch (call) {
    case Shared::IpcCall::WriteToStorage:
      ValidateAndCall(api::WriteToStorage);
      break;
    case Shared::IpcCall::ReadFromStorage:
      ValidateAndCall(api::ReadFromStorage);
      break;
    case Shared::IpcCall::DeleteFromStorage:
      ValidateAndCall(api::DeleteFromStorage);
      break;
  }

  return resp;
}

crow::response keepalive() {
  quitting::keep_alive();
  crow::response resp;
  resp.code = 200;
  resp.body = "OK";
  resp.set_header("Content-Type", "text/plain");
  return resp;
}

crow::response quit() {
  quitting::really_quit();
  return crow::response(200);
}

void socket_message(crow::websocket::connection& conn,
                    const std::string& data,
                    bool is_binary) {
  std::cout << "Got a message from the client:";
  std::cout << data << std::endl;
  // Message is IpcMessage;[json-formatted array of arguments]
  size_t pos = data.find(';');
  if (pos == data.npos) {
    std::cerr << "Invalid websocket message received: " << data << std::endl;
    return;
  }
  auto maybeNum = tools::read_uint64_t(std::string_view{data.c_str(), pos});
  if (!maybeNum) {
    std::cerr << "Invalid websocket message received: " << data << std::endl;
    return;
  }
  Shared::IpcMsg msg = static_cast<Shared::IpcMsg>(*maybeNum);
  if (!Shared::is_valid(msg)) {
    std::cerr << "Invalid websocket message received: " << data << std::endl;
    return;
  }
  switch (msg) {
    case Shared::IpcMsg::ManualRescan:
      std::cout << "TODO: Implement ManualRescan" << std::endl;
      break;
    default:
      std::cerr << "Unsupported message received: " << val << " (" << data
                << ")" << std::endl;
  }
}

} // namespace handlers