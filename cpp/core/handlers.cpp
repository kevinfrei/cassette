#include <cctype>
#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include <crow.h>

#include "CommonTypes.hpp"
#include "api.hpp"
#include "config.hpp"
#include "files.hpp"
#include "handlers.hpp"
#include "musicdb.hpp"
#include "quitting.hpp"
#include "setup.hpp"
#include "tools.hpp"
#include "tunes.hpp"

namespace handlers {

crow::response file_path(const crow::request&, const std::string& path) {
  quitting::keep_alive();

  std::cout << "Path: " << path << std::endl;
  crow::response resp;
  std::filesystem::path p =
      files::get_web_dir() / (path.empty() ? "index.html" : path);
  if (p.filename() == "index.html") {
    // If we're sending index.html, we should *clear* ready
    config::not_ready();

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
    std::string wsport = std::to_string(setup::get_random_port());
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
    resp.set_header("Content-type", files::path_to_mime_type(p));
  }
  return resp;
}

crow::response images(const crow::request&, const std::string& /*path*/) {
  quitting::keep_alive();
  crow::response resp;
  /*
  std::filesystem::path p = files::get_web_dir() / "images" / path;
  if (!std::filesystem::exists(p)) {
    tools::e404(resp, "Image not found");
    return resp;
  }*/
  // TODO: This is clearly bad & wrong, but I don't care right now.
  std::filesystem::path p = config::get_home_path() / "src" / "cassette" /
                            "www" / "img" / "album.svg";
  resp.set_static_file_info_unsafe(p.generic_string());
  resp.set_header("Content-type", files::path_to_mime_type(p));
  return resp;
}

// TODO: Make this actually validate the Range header
struct range_header {
  std::uint64_t start;
  std::uint64_t end;
  bool start_present;
  bool end_present;
};

std::optional<range_header> validate_range_header(const std::string& range) {
  std::optional<range_header> res;
  if (range.substr(0, 6) != "bytes=") {
    return res;
  }
  size_t dashPos = range.find('-');
  if (dashPos == std::string::npos) {
    return res;
  }
  range_header rh;
  rh.start_present = false;
  rh.end_present = false;
  if (dashPos > 6) {
    rh.start = std::stoull(range.substr(6, dashPos - 6));
    rh.start_present = true;
  }
  if (dashPos + 1 < range.size()) {
    rh.end = std::stoull(range.substr(dashPos + 1));
    rh.end_present = true;
  }
  if (rh.start_present || rh.end_present) {
    res = rh;
  }
  return res;
}

crow::response tune(const crow::request& req, const std::string& path) {
  quitting::keep_alive();
  crow::response resp;
  auto maybe_song = tunes::get_tune(path);
  if (!maybe_song) {
    tools::e404(resp, "Tune not found");
    return resp;
  }
  /*
  A minimal implementation needs to:
  Parse the Range header
  Compute the correct byte offsets
  Return 206 Partial Content
  Include Content-Range and Accept-Ranges: bytes
  Send only the requested slice of the file
  */
  const auto& range = req.headers.find("Range");
  range_header rh;
  if (range != req.headers.end()) {
    std::cout << "Range header: " << range->second << std::endl;
    auto maybe_range = validate_range_header(range->second);
    if (!maybe_range.has_value()) {
      // TODO: Handle weirder ranges?
      resp.code = 416;
      return resp;
    } else {
      rh = maybe_range.value();
    }
  }
  const auto& song = maybe_song.value();
  // TODO: Get the file size, check to see we can send the amount requested.
  // If we can, go ahead & send it.
  // Common case: Safari asks for 0-1 for audio files, presumably to detect
  // the total file size?
  if (rh.start_present && rh.end_present && rh.start == 0 && rh.end == 1) {
    // Send the two starting bytes for 'song':
    std::ifstream file(song, std::ios::binary);
    if (file.is_open()) {
      char buffer[2];
      file.read(buffer, 2);
      resp.body = std::string(buffer, 2);
      resp.code = 206;
      resp.set_header("Content-Type", files::path_to_mime_type(song));
      resp.set_header("Accept-Ranges", "bytes");
      resp.set_header(
          "Content-Range",
          "bytes 0-1/" + std::to_string(std::filesystem::file_size(song)));
      return resp;
    }
  }
  resp.set_static_file_info_unsafe(song.generic_string());
  resp.set_header("Content-type", files::path_to_mime_type(song));
  resp.set_header("Accept-Ranges", "bytes");
  std::size_t fileSize = std::filesystem::file_size(song);
  std::ostringstream o;
  o << "bytes 0-" << fileSize - 1 << "/" << fileSize;
  std::cout << o.str() << std::endl;
  resp.set_header("Content-Range", o.str());
  resp.code = 206; // Partial Content
  return resp;
}

crow::response api(const crow::request&, const std::string& path) {
  quitting::keep_alive();

  // std::cout << "API Path: " << path << std::endl;
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
      handle_call(resp,
                  std::string_view{path.c_str() + slashPos + 1,
                                   path.size() - slashPos - 1});
    }
  };
  switch (call) {
    case Shared::IpcCall::WriteToStorage:
      ValidateAndCall(api::write_to_storage);
      break;
    case Shared::IpcCall::ReadFromStorage:
      ValidateAndCall(api::read_from_storage);
      break;
    case Shared::IpcCall::DeleteFromStorage:
      ValidateAndCall(api::delete_from_storage);
      break;
    case Shared::IpcCall::ShowOpenDialog:
      files::folder_picker(resp,
                           std::string_view{path.c_str() + slashPos + 1,
                                            path.size() - slashPos - 1});
      break;
    default:
      if (Shared::is_valid(call)) {
        std::cerr << "Unimplemented API call received: "
                  << Shared::to_string(call) << " ("
                  << static_cast<std::underlying_type_t<Shared::IpcCall>>(call)
                  << ") [" << path << "]" << std::endl;
      } else {
        std::cerr
            << "Unknown API call received: " << static_cast<std::uint64_t>(call)
            << " [" << path << "]" << std::endl;
      }

      tools::e404(resp, "Unknown/Unimplemented API call");
      return resp;
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
                    bool /* is_binary */) {
  std::cout << "Got a message from the client:";
  std::cout << data << std::endl;
  // Message is IpcMessage;[json-formatted array of arguments]
  size_t pos = data.find(';');
  if (pos == data.npos) {
    std::cerr << "Invalid websocket message received: " << data << std::endl;
    return;
  }
  auto maybeMsg = tools::read_uint64_t(std::string_view{data.c_str(), pos});
  if (!maybeMsg) {
    std::cerr << "Invalid websocket message received: " << data << std::endl;
    return;
  }
  Shared::SocketMsg msg = static_cast<Shared::SocketMsg>(*maybeMsg);
  if (!Shared::is_valid(msg)) {
    std::cerr << "Invalid Socket message received: " << data << std::endl;
    return;
  }
  // This is the only message we support *receiving* from the client
  switch (msg) {
    case Shared::SocketMsg::ManualRescan:
      std::cout << "TODO: Implement ManualRescan" << std::endl;
      break;
    case Shared::SocketMsg::ContentLoaded:
      config::set_ready();
      tunes::send_music_db(conn);
      break;
    default: // Unsupported message
      std::cerr << "Unsupported message received: " << Shared::to_string(msg)
                << " ("
                << static_cast<std::underlying_type_t<Shared::SocketMsg>>(msg)
                << ") [" << data << "]" << std::endl;
  }
}

} // namespace handlers