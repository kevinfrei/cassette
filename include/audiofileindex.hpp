#if !defined(AUDIOFILEINDEX_HPP)
#define AUDIOFILEINDEX_HPP
#pragma once

#include <crow.h>

#include "CommonTypes.hpp"

namespace afi {

Shared::MusicDatabase* get_music_db();
void send_music_db(crow::websocket::connection& conn);

} // namespace afi

#endif // AUDIOFILEINDEX_HPP