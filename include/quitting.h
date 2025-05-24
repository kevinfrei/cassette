#include <atomic>

extern std::atomic_bool quit;

void keep_alive();
bool should_quit();
void really_quit();
void loop_wait();