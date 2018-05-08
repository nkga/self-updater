#include "client.h"
#include "loader.h"
#include <Windows.h>

// Nothing too interesting here. Just running the client.

static core::client_t* client;

void exp_startup() {
	client = new core::client_t();
}

void exp_cleanup() {
	delete client;
}

loader::action_t exp_update() {
	return client->update();
}

BOOL WINAPI DllMain(HANDLE module, DWORD reason, LPVOID reserved) {
	return TRUE;
}
