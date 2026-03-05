#if !defined(__TUNES_H__)
#define __TUNES_H__
#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include <crow.h>

#include "CommonTypes.hpp"

namespace tunes {

std::optional<std::filesystem::path> get_tune(const std::string& path);
const Shared::MusicDatabase& get_music_db();
void send_music_db(crow::websocket::connection& conn);

} // namespace tunes

#endif // __TUNES_H__
