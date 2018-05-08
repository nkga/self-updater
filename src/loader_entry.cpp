#include "loader.h"
#include "core_filesystem.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>

using namespace core;

namespace core {

static bool find_func(HMODULE mod, LPVOID func, LPCSTR name) {
	FARPROC* fp = (FARPROC*)func;
	*fp = mod ? GetProcAddress(mod, name) : 0;
	return (fp != 0);
}

}

int main(int argc, char** argv) {
	static WCHAR tmp_dll_path[512];

	GetTempPathW(MAX_PATH, tmp_dll_path);
	PathAppendW(tmp_dll_path, L"client.dll.tmp");

	void(*exp_startup)();
	void(*exp_cleanup)();
	loader::action_t(*exp_update)();

	bool exit = false;

	while (exit == false) {
		if (filesystem::copy(L"client.dll", tmp_dll_path) == false) {
			wprintf(L"error: couldn't copy client dll\n");
			return EXIT_FAILURE;
		}

		HMODULE hmodule = LoadLibraryW(tmp_dll_path);
		if (hmodule == 0) {
			wprintf(L"error: missing client dll.\n");
			return EXIT_FAILURE;
		}

		if (find_func(hmodule, &exp_startup, "exp_startup") == false) {
			wprintf(L"error: missing exp_startup");
			return EXIT_FAILURE;
		}

		if (find_func(hmodule, &exp_cleanup, "exp_cleanup") == false) {
			wprintf(L"error: missing exp_cleanup");
			return EXIT_FAILURE;
		}

		if (find_func(hmodule, &exp_update, "exp_update") == false) {
			wprintf(L"error: missing exp_update");
			return EXIT_FAILURE;
		}

		exp_startup();

		for (;;) {
			auto action = exp_update();
			if (action == loader::action_t::reload) {
				break;
			} else if (action == loader::action_t::exit) {
				exit = true;
				break;
			}
		}

		exp_cleanup();

		FreeLibrary(hmodule);
	}

	return EXIT_SUCCESS;
}
