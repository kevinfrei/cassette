#pragma once

#include <optional>
#include <string>
#include <vector>

#include "CommonTypes.hpp"

namespace playlist {

std::optional<std::vector<std::string>> get_names();
std::optional<std::vector<Shared::SongKey>> load(std::string_view name);
void save(std::string_view name, const std::vector<std::string_view>& items);

} // namespace playlist
