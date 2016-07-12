#include "Helper.h"

#include <chrono>


// seed the rng with the time
Rng Helper::rng_ = Rng(static_cast<RngInt>(std::chrono::system_clock::now().time_since_epoch().count()));