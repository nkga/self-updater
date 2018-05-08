#pragma once
#include "core.h"
#include "core_heap.h"
#include <Windows.h>

namespace core {
namespace filesystem {

// A file on disk.
class file_t {
public:
	// Initializes the file.
	file_t();

	// Closes the file if it was left open.
	~file_t();

	// Closes the file.
	void close();

	// Opens the file.
	bool open(DWORD access, DWORD disposition, WCHAR const* path);

	// Returns true if the file is valid.
	bool valid() const;

	// Seeks the file. Returns the new seek position in bytes.
	i64 seek(i64 offset, DWORD origin);

	// Returns the file size in bytes.
	i64 size();

	// Reads from the file. Returns the number of bytes read.
	u32 read(void* dst, u32 bytes);

	// Writes to the file. Returns the number of bytes written.
	u32 write(void const* src, u32 bytes);

	// Formats a string and writes it to the file. Returns the number of bytes written.
	u32 print(CHAR const* fmt, ...);

protected:
	HANDLE handle;
};

// File contents buffer.
struct buffer_t {
	BYTE* data;
	u32 size;

	// Initializes the buffer.
	inline buffer_t() : data(nullptr), size(0) {
	}

	// Frees the buffer contents, if any.
	inline ~buffer_t() {
		free();
	}

	// Frees the buffer contents, if any.
	inline void free() {
		heap::free(&data);
		size = 0;
	}
};

// Copies a file from the src path to the dst path.
bool copy(WCHAR const* src_path, WCHAR const* dst_path);

// Reads a file from disk into a buffer. Returns true on success.
bool read(buffer_t* buffer, WCHAR const* path);

// Writes data to disk.
bool write(void const* src, u32 src_bytes, WCHAR const* path);

// Returns the last modify time for a file, in milliseconds.
i64 get_write_ms(WCHAR const* paths);

}
}

