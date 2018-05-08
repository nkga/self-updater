#include "core_hash.h"
#include <assert.h>

namespace core {
namespace hash {

u32 crc32(void const* src, u32 bytes) {
	assert(src);

	static u32 const lut[16] = {
		0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190,
		0x6B6B51F4, 0x4DB26158, 0x5005713C, 0xEDB88320, 0xF00F9344,
		0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278,
		0xBDBDF21C
	};

	u32 crc = 0xFFFFFFFF;
	BYTE const* ptr = (BYTE const*)src;

	while (bytes--) {
		crc = lut[(crc ^ *ptr) & 0x0F] ^ (crc >> 4);
		crc = lut[(crc ^ (*ptr >> 4)) & 0x0F] ^ (crc >> 4);
		ptr++;
	}

	return ~crc;
}

u32 mix32(u32 x) {
	x = ((x >> 16) ^ x) * 0x45D9F3Bu;
	x = ((x >> 16) ^ x) * 0x45D9F3Bu;
	x = ((x >> 16) ^ x);
	return x;
}

u64 mix64(u64 x) {
	x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9llu;
	x = (x ^ (x >> 27)) * 0x94D049BB133111EBllu;
	x = (x ^ (x >> 31));
	return x;
}

u64 raw64(void const* src, u32 bytes) {
	assert(src);

	constexpr const u32 PRIME32_1 = 2654435761U;
	constexpr const u32 PRIME32_2 = 2246822519U;
	constexpr const u32 PRIME32_3 = 3266489917U;
	constexpr const u32 PRIME32_4 = 668265263U;
	constexpr const u32 PRIME32_5 = 374761393U;

	const BYTE* p = (const BYTE*)src;
	const BYTE* bEnd = p + bytes;
	u32 h32 = PRIME32_5 + (u32)bytes;

	while (p + 4 <= bEnd) {
		h32 += (*(const u32*)p) * PRIME32_3;
		h32 = ((h32 << 17) | (h32 >> (32 - 17))) * PRIME32_4;
		p += 4;
	}

	while (p < bEnd) {
		h32 += (*p) * PRIME32_5;
		h32 = ((h32 << 11) | (h32 >> (32 - 11))) * PRIME32_1;
		p++;
	}

	h32 ^= h32 >> 15;
	h32 *= PRIME32_2;
	h32 ^= h32 >> 13;
	h32 *= PRIME32_3;
	h32 ^= h32 >> 16;

	return h32;
}

u32 text(CHAR const * src) {
	assert(src);

	u32 x = 0x811C9DC5u;

	for (;;) {
		u32 v = *src++;
		if (v == 0) {
			break;
		}

		x = x ^ v;
		x = x * 0x1000193u;
	}

	return x;
}

}
}
