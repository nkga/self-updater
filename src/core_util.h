#pragma once
#include "core.h"

namespace core {
namespace util {

// Returns the number of elements in a fixed sized array.
template <typename T, size_t n>
constexpr size_t count(T const (&x)[n]) {
	return n;
}

// Returns the maximum of x or y for a value type.
template <typename type_t>
constexpr type_t max(type_t x, type_t y) {
	return (x < y) ? y : x;
}

// Returns the mininum of x or y for a value type.
template <typename type_t>
constexpr type_t min(type_t x, type_t y) {
	return (y < x) ? y : x;
}

}
}
