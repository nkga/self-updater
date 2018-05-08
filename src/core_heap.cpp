#include "core_heap.h"
#include <Windows.h>

namespace core {
namespace heap {

void raw_free(void* ptr) {
	if (ptr) {
		HeapFree(GetProcessHeap(), 0, ptr);
	}
}

void* raw_alloc(size_t bytes) {
	return HeapAlloc(GetProcessHeap(), 0, bytes);
}

void* raw_calloc(size_t bytes) {
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes);
}

void* raw_realloc(void* ptr, size_t bytes) {
	HANDLE heap = GetProcessHeap();

	void* res;
	if (ptr) {
		res = HeapReAlloc(heap, 0, ptr, bytes);
	} else {
		res = HeapAlloc(heap, 0, bytes);
	}

	return res;
}

}
}

void* operator new(size_t size) {
	return core::heap::raw_alloc(size);
}

void operator delete(void* ptr) {
	core::heap::raw_free(ptr);
}
