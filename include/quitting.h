#include <atomic>

extern std::atomic_bool quit;
extern std::atomic_int8_t quit_timer;

void keep_alive();
