#include <string>

namespace window {

void open(const std::string& url) {
  std::string cmdln;
#if defined(_WIN32)
  cmdln = "start ";
#elif defined(__APPLE__)
  cmdln =
      "/Applications/Google\\ Chrome.app/Contents/MacOS/Google\\ Chrome "
      "--new-window --app=";
#elif defined(__linux__)
  cmdln = "xdg-open ";
#else
  std::cout << "Please open " << url << " in your browser." << std::endl;
  return;
#endif
  cmdln += url;
  system(cmdln.c_str());
}

} // namespace window