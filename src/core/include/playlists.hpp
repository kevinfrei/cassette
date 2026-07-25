#pragma once

#include <optional>
#include <string>
#include <vector>

#include "CommonTypes.hpp"

namespace playlist {

std::optional<std::vector<std::string>> get_names();
std::optional<std::vector<Shared::SongKey>> load(const std::string& name);
void save(const std::string& name, const std::vector<std::string>& items);
void remove(const std::string& name);
} // namespace playlist
