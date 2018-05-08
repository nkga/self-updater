#pragma once
#include "core.h"

namespace core {
namespace hash {

u32 crc32(void const* src, u32 bytes);
u32 mix32(u32 x);
u64 mix64(u64 x);
u64 raw64(void const* src, u32 bytes);
u32 text(CHAR const* src);

}
}
