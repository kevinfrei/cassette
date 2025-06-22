#include <filesystem>
#include <string>

namespace files {

void set_program_location();
std::string path_to_mime_type(const std::filesystem::path& file_path);
std::filesystem::path get_web_dir();
std::filesystem::path get_app_name();

} // namespace files