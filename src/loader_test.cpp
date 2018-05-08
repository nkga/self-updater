#include <Windows.h>
#include <Shlwapi.h>

static HMODULE find_dll() {
	static HMODULE original_dll;
	static BOOL load_done;

	if (load_done == FALSE) {
		load_done = TRUE;

		WCHAR path[MAX_PATH];
		GetSystemDirectoryW(path, ARRAYSIZE(path));
		PathAppendW(path, L"d3d9.dll");
		original_dll = LoadLibraryW(path);
	}

	return original_dll;
}

static bool find_func(LPVOID func, LPCSTR name) {
	auto dll = find_dll();
	if (dll == 0) {
		return false;
	}

	auto fp = (FARPROC*)func;
	if (*fp == nullptr) {
		return false;
	}
	
	*fp = GetProcAddress(dll, name);

	return (fp != nullptr);
}

DWORD WINAPI D3DPERF_GetStatus(void) {
	static decltype(&D3DPERF_GetStatus) func;

	if (find_func(&func, "D3DPERF_GetStatus") == false) {
		return 0;
	}

	return func();
}