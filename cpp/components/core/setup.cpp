
#include <cstdint>
#include <random>

namespace {
uint16_t port = 0;
}

uint16_t get_random_port() {
  if (port == 0) {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 16383);
    port = 49152 + static_cast<uint16_t>(dist(rd)); // Use ports in the range
                                                    // 49152-65535
  }
  return port;
}
