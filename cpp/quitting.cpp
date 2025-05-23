#include "quitting.h"

std::atomic_bool quit = false;
std::atomic_int8_t quit_timer = 10;
bool really_quit = !_DEBUG;

void keep_alive() {
  quit_timer.store(10);
}

bool should_quit() {
  return quit && really_quit;
}

void loop_wait() {
  if (quit_timer > 0) {
    quit_timer--;
  } else {
    quit = true;
  }
}