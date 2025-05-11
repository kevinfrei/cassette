#define _WIN32_WINNT 0xA00

#include "crow.h"
#include <iostream>

int main(void) {
  crow::SimpleApp app;
  // Define a route
  app.route<crow::black_magic::get_parameter_tag("/")>("/")([] {
    return "<html>"
           "<head><title>Hi planet</title></head>"
           "<body><p>Hello, World!</p></body>"
           "</html>";
  });
  std::cout << "http://localhost:18080" << std::endl;
#if defined(_WIN32)
  system("start http://localhost:18080");
#elif defined(__APPLE__)
  system("open http://localhost:18080");
#elif defined(__linux__)
  system("xdg-open http://localhost:18080");
#else
  std::cout << "Please open http://localhost:18080 in your browser."
            << std::endl;
#endif
  app.port(18080).multithreaded().run();
  return 0;
}
