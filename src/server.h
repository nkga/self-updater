#pragma once
#include "core_crypto.h"
#include "core_filesystem.h"
#include "core_socket.h"
#include "server_clients.h"
#include "server_iptable.h"

namespace core {

class server_t {
public:
	// Creates the server.
	server_t();

	// Destroys the server.
	~server_t();

	// Runs the server.
	void run();

protected:
	// Path to the update file data.
	static WCHAR const update_file_path[];

	// Path to the update signature data.
	static WCHAR const update_sig_path[];

	// Network step. Returns true if the socket is still valid.
	bool step();

	// Handles a client update message.
	void msg_client_update_begin(BYTE* buf, u32 len, addr_t addr);

	// Handles a client piece message.
	void msg_client_update_piece(BYTE* buf, u32 len, addr_t addr);

	// Refreshes the update data if needed.
	bool update_refresh(i64 now_ms);

	// Subsystems.
	crypto_t crypto;
	socket_t socket;
	iptable_t iptable;
	clients_t clients;

	// Update data.
	i64 last_update_check_ms;
	i64 last_update_write_ms;
	u32 update_version;
	msg::sig_t update_sig;
	filesystem::buffer_t update_file;
};

}