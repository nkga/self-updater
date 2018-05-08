#pragma once
#include "core.h"

namespace core {
namespace timer {

// Returns the current time in milliseconds.
i64 query_ms();

// Sleeps for the given number of milliseconds.
void sleep_ms(u32 ms);

}
}
