#include "quitting.h"

std::atomic_bool quit = false;
std::atomic_int8_t quit_timer = 10;

void keep_alive() {
  quit_timer.store(10);
}
