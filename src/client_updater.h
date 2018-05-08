#pragma once
#include "core.h"
#include "core_filesystem.h"
#include "core_socket.h"
#include "core_heap.h"
#include "core_msg.h"
#include <vector>

namespace core {

class crypto_t;
class socket_t;

class updater_t {
public:
	// Update status codes.
	enum status_code_t {
		STATUS_NONE, // No error code.
		STATUS_BAD_CRC, // CRC check failed.
		STATUS_BAD_SIG, // Signature check failed.
		STATUS_BAD_WRITE, // Write to disk failed.
	};

	// Update status.
	struct status_t {
		u32 bytes;
		u32 total;
		status_code_t code;
	};

	// Initializes the updater.
	updater_t(crypto_t* crypto, socket_t* socket);

	// Destroys the updater.
	~updater_t();

	// Returns the current update status.
	status_t get_status();

	// Handles the update token message from the server.
	void handle_msg_token(BYTE const* buf, u32 len);

	// Handles the update begin message from the server.
	void handle_msg_begin(BYTE const* buf, u32 len);

	// Handles the update piece message from the server.
	void handle_msg_piece(BYTE const* buf, u32 len);

	// Returns true if the updater requires a restart.
	bool needs_restart();

	// Resets the current update.
	void reset();

	// Updates the updater state.
	void update(i64 now_ms, addr_t host);

protected:
	// Path for the update data.
	static WCHAR const update_path[];

	// Public key data.
	static BYTE const public_key[];

	// Maximum number of pieces to request per update.
	static constexpr u32 max_requests = 32;

	// Update buffer.
	struct buffer_t {
		BYTE* ptr;
		u32 size;

		// Creates the buffer.
		buffer_t();

		// Destroys the buffer.
		~buffer_t();

		// Allocates space in the buffer. Returns true on success.
		bool alloc(u32 bytes);

		// Frees the buffer. Zeroes the contents before free.
		void free();
	};

	crypto_t* crypto;
	socket_t* socket;
	i64 last_update_ms;
	i64 last_piece_ms;
	u64 client_token;
	u64 server_token;
	u32 client_version;
	u32 server_version;
	status_t status;
	bool is_finished;
	bool is_updating;
	u32 update_idx;
	buffer_t update_data;
	buffer_t update_pieces;
	msg::sig_t update_sig;
};

}
