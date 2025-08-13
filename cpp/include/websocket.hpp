#if !defined(__WEBSOCKET_H__)
#define __WEBSOCKET_H__
#pragma once

#include <crow/app.h>

namespace websocket {
void configure(crow::SimpleApp& app);
void keep_alive();
} // namespace websocket

#endif