#include "core_timer.h"
#include <Windows.h>

namespace core {
namespace timer {

i64 query_ms() {
	return (i64)GetTickCount64();
}

void sleep_ms(u32 ms) {
	Sleep(ms);
}

}
}
