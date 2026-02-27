#include <optional>
#include <string>
#include <vector>

#include "playlists.hpp"

namespace playlist {

std::optional<std::vector<std::string>> get_names() {
  return std::vector<std::string>{"My Playlist1", "My Playlist2"};
}

std::optional<std::vector<Shared::SongKey>> load(std::string_view name) {
  if (name == "My Playlist1") {
    return std::vector<Shared::SongKey>{"S1", "S2"};
  } else if (name == "My Playlist2") {
    return std::vector<Shared::SongKey>{"S3", "S4", "S2"};
  }
  return std::nullopt;
}

} // namespace playlist