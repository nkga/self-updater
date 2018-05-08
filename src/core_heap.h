#pragma once
#include "core.h"

namespace core {
namespace heap {

void raw_free(void* ptr);
void* raw_alloc(size_t bytes);
void* raw_calloc(size_t bytes);
void* raw_realloc(void* ptr, size_t bytes);

template <typename tval>
inline tval* alloc(size_t count = 1) {
	return (tval*)raw_alloc(count * sizeof(tval));
}

template <typename tval>
inline tval* calloc(size_t count = 1) {
	return (tval*)raw_calloc(count * sizeof(tval));
}

template <typename tval>
inline tval* free(tval** ptr) {
	raw_free(*ptr);
	*ptr = nullptr;
}

template <typename tval>
inline tval* realloc(tval* ptr, u32 count) {
	return (tval*)raw_realloc(ptr, count * sizeof(tval));
}

}
}
