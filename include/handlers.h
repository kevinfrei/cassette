#include <string>

#include <crow/http_request.h>
#include <crow/http_response.h>

crow::response handle_file_path(const crow::request& req,
                                const std::string& path);
crow::response handle_tune(const crow::request& req, const std::string& path);
crow::response handle_api(const crow::request& req, const std::string& path);