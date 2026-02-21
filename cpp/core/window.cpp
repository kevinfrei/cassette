#include <string>

#include "webview.h"

namespace window {

// TODO: This is *very* stupid, and requires Chrome is installed in the
// default location. I should, instead, detect if Chrome is installed in a
// much more robust fashion. If it's not, then I should fall back to just
// opening the URL
void open(const std::string& url) {
#if 0
  std::string cmdln;
#if defined(_WIN32)
  cmdln = "start ";
  /*
  cmdln =
      "\"c:\\Program Files\\Google\\Chrome\\Application\\chrome.exe\" "
      "--new-window --app=";
    */
#elif defined(__APPLE__)
  cmdln = "open "; 
  /*
  cmdln =
      "/Applications/Google\\ Chrome.app/Contents/MacOS/Google\\ Chrome "
      "--new-window --app="; 
  */
#elif defined(__linux__)
  // /opt/google/chrome/chrome --new-window --app=
  cmdln = "xdg-open ";
#else
  std::cout << "Please open " << url << " in your browser." << std::endl;
  return;
#endif
  cmdln += url;
  system(cmdln.c_str());
#endif
  // 3. Initialize Webview on the Main Thread
  webview::webview w(true, nullptr);
  w.set_title("Async Local Server");
  w.set_size(800, 600, WEBVIEW_HINT_NONE);

  w.navigate(url);
  w.run(); // This blocks until the window is closed
}

} // namespace window