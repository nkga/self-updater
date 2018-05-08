#include "core_filesystem.h"
#include "core_heap.h"
#include <Shlwapi.h>
#include <stdio.h>
#include <assert.h>

namespace core {
namespace filesystem {

file_t::file_t() : handle(INVALID_HANDLE_VALUE) {
}

file_t::~file_t() {
	close();
}

void file_t::close() {
	if (handle != INVALID_HANDLE_VALUE) {
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
}

bool file_t::open(DWORD access, DWORD disposition, WCHAR const* path) {
	handle = CreateFileW(path, access, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, disposition, FILE_ATTRIBUTE_NORMAL, NULL);
	return (handle != INVALID_HANDLE_VALUE);
}

bool file_t::valid() const {
	return (handle != INVALID_HANDLE_VALUE);
}

i64 file_t::seek(i64 offset, DWORD origin) {
	LARGE_INTEGER loff, lpos;
	loff.QuadPart = offset;
	return handle && SetFilePointerEx(handle, loff, &lpos, origin) ? lpos.QuadPart : 0;
}

i64 file_t::size() {
	LARGE_INTEGER result;
	return handle && GetFileSizeEx(handle, &result) ? result.QuadPart : 0;
}

u32 file_t::read(void* dst, u32 bytes) {
	assert(dst);
	assert(bytes);

	DWORD result;
	return ReadFile(handle, dst, bytes, &result, 0) ? result : 0;
}

u32 file_t::write(void const* src, u32 bytes) {
	assert(src);
	assert(bytes);


	DWORD result;
	return WriteFile(handle, src, bytes, &result, 0) ? result : 0;
}

u32 file_t::print(CHAR const* fmt, ...) {
	assert(fmt);

	va_list args;
	va_start(args, fmt);

	CHAR buf[4096];
	i32 res = vsnprintf(buf, sizeof(buf), fmt, args);
	if (res >= sizeof(buf)) {
		// truncation
		res = sizeof(buf) - 1;
	}

	va_end(args);

	if (res <= 0) {
		return 0;
	}

	return write(buf, (u32)res);
}

bool copy(WCHAR const* src_path, WCHAR const* dst_path) {
	assert(src_path);
	assert(dst_path);

	if (src_path == 0 || src_path[0] == 0) {
		return false;
	}

	DWORD base_attr = GetFileAttributesW(src_path);
	if (base_attr & FILE_ATTRIBUTE_READONLY) {
		return false;
	}

	return CopyFileW(src_path, dst_path, FALSE);
}

bool read(buffer_t* buffer, WCHAR const* path) {
	assert(buffer);
	assert(path);

	bool result = false;

	file_t file;
	if (file.open(GENERIC_READ, OPEN_EXISTING, path)) {
		auto real_size = file.size();
		if (real_size > 0 && real_size < 0xFFFFFFFF) {
			auto file_size = (u32)real_size;
			auto file_data = heap::calloc<BYTE>(real_size + 16); // SIMD alignment bonus.
			if (file_data) {
				if (file.read(file_data, file_size) == file_size) {
					buffer->data = file_data;
					buffer->size = file_size;
					result = true;
				} else {
					heap::raw_free(file_data);
				}
			}
		}
	}

	return result;
}

bool write(void const* src, u32 src_bytes, WCHAR const* path) {
	assert(src);
	assert(src_bytes);
	assert(path);

	bool result = false;

	file_t file;
	if (file.open(GENERIC_WRITE, CREATE_ALWAYS, path)) {
		result = (file.write(src, src_bytes) == src_bytes);
	}

	return result;
}

i64 get_write_ms(WCHAR const* path) {
	assert(path);

	WIN32_FILE_ATTRIBUTE_DATA wfd = {};

	if (GetFileAttributesExW(path, GetFileExInfoStandard, &wfd) == FALSE) {
		return 0;
	}

	ULARGE_INTEGER ui;
	ui.LowPart = wfd.ftLastWriteTime.dwLowDateTime;
	ui.HighPart = wfd.ftLastWriteTime.dwHighDateTime;
	ui.QuadPart /= 10000;

	return ui.QuadPart;
}

}
}
