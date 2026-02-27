#if !defined(SETUP_HPP)
#define SETUP_HPP

#pragma once

#include <cstdint>

namespace setup {

std::uint16_t get_random_port();
void init();
void run();

} // namespace setup

#endif // SETUP_HPP
