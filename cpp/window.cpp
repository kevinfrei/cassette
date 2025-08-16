#include <string>

namespace window {

// TODO: This is *very* stupid, and requires Chrome is installed in the
// default location. I should, instead, detect if Chrome is installed in a
// much more robust fashion. If it's not, then I should fall back to just
// opening the URL
void open(const std::string &url) {
  std::string cmdln;
#if defined(_WIN32)
  cmdln = "\"c:\\Program Files\\Google\\Chrome\\Application\\chrome.exe\" "
          "--new-window --app=";
#elif defined(__APPLE__)
  cmdln = "/Applications/Google\\ Chrome.app/Contents/MacOS/Google\\ Chrome "
          "--new-window --app=";
#elif defined(__linux__)
  // /opt/google/chrome/chrome --new-window --app=
  cmdln = "xdg-open ";
#else
  std::cout << "Please open " << url << " in your browser." << std::endl;
  return;
#endif
  cmdln += url;
  system(cmdln.c_str());
}

} // namespace window