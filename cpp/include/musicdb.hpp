#if !defined(MUSICDB_HPP)
#define MUSICDB_HPP
#pragma once

#include <crow.h>

#include "CommonTypes.hpp"

namespace musicdb {

Shared::MusicDatabase* get_music_db();
void send_music_db(crow::websocket::connection& conn);

} // namespace musicdb

#endif // MUSICDB_HPP