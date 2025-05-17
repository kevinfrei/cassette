#if defined(_WIN32)
#define _WIN32_WINNT 0xA00
#endif

#include "crow.h"

#include <filesystem>
#include <iostream>

std::string ExtensionToMimeType(const std::string& extension) {
  if (extension == "txt") {
    return "text/plain";
  } else if (extension == "css" || extension == "scss") {
    return "text/css";
  } else if (extension == "html" || extension == "htm") {
    return "text/html";
  } else if (extension == "js") {
    return "text/javascript";
  } else if (extension == "gif") {
    return "image/gif";
  } else if (extension == "jpg") {
    return "image/jpeg";
  } else if (extension == "png") {
    return "image/png";
  } else if (extension == "svg") {
    return "image/svg";
  } else if (extension == "flac") {
    return "audio/flac";
  } else if (extension == "m4a") {
    return "audio/m4a";
  } else if (extension == "mp3") {
    return "audio/mp3";
  }
  return "text/html";
}

std::string LoadFileWithMimeType(const std::filesystem::path& path,
                                 std::string& mime_type) {
  std::ifstream input_file(path, std::ifstream::binary);

  // Get file size
  input_file.seekg(0, input_file.end);
  size_t file_size = input_file.tellg();
  input_file.seekg(0, input_file.beg);

  std::string ret;
  ret.resize(file_size);
  input_file.read(ret.data(), file_size);
  input_file.close();

  mime_type = ExtensionToMimeType(path.extension().string());

  return ret;
}

int main(void) {
  crow::SimpleApp app;
  // Define a route
  CROW_ROUTE(app, "/<path>")
  ([&](const crow::request& req, const std::string& path) {
    crow::response resp;
    std::filesystem::path p = std::filesystem::path{"www"};
    if (path.empty()) {
      p = p / "index.html";
    } else {
      p = p / path;
    }
    std::string mime_type = ExtensionToMimeType(p.extension().string());
    resp.set_static_file_info(p.generic_string());
    // resp = LoadFileWithMimeType(p / path, mime_type);
    // resp.set_header("Content-Type", mime_type);
    // resp.end();
    return resp;
  });
  // CROW_ROUTE(app, "/")([] {
  //   crow::response res("<html>"
  //     "<head><title>Simple Crow App</title></head>"
  //     "<body>"
  //     "<h1>Hello, Crow!</h1>"
  //     "<a href='app/index.html'>The App</a>"
  //     "</body>"
  //     "</html>"
  //   );
  //   // res.set_header("Content-Type", "text/html");
  //   res.end();
  //   return res;
  // });
  std::cout << "http://localhost:18080/index.html" << std::endl;
#if defined(_WIN32)
  system("start http://localhost:18080/index.html");
#elif defined(__APPLE__)
  system("open http://localhost:18080/index.html");
#elif defined(__linux__)
  system("xdg-open http://localhost:18080/index.html");
#else
  std::cout << "Please open http://localhost:18080/index.html in your browser."
            << std::endl;
#endif
  app.port(18080).multithreaded().run();
  return 0;
}
