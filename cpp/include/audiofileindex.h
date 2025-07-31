#include <filesystem>

// Stuff for the afi implemention only?

namespace afi {

class audio_file_index {
  static std::unordered_set<audio_file_index*> afis;

  std::filesystem::path loc;

  audio_file_index(const std::filesystem::path& loc);

 public:
  static audio_file_index* create(const std::filesystem::path& loc);
};

} // namespace afi