#include <string>

#include <crow/http_request.h>
#include <crow/http_response.h>

namespace handlers {

crow::response file_path(const crow::request& req, const std::string& path);
crow::response tune(const crow::request& req, const std::string& path);
crow::response api(const crow::request& req, const std::string& path);
crow::response keepalive();
crow::response quit();

} // namespace handlers