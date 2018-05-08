#include "client.h"
#include "core_msg.h"
#include "core_timer.h"

namespace core {

client_t::client_t() : updater(&crypto, &socket) {
	// The client is non-blocking since it updates in a loop.
	socket.open();
	socket.set_non_blocking(true);

	// Get the address for the server.
	host = socket.lookup(msg::net_host, msg::net_port);

	printf("build %s version %u client\n\n", __TIMESTAMP__, msg::VERSION);
}

client_t::~client_t() {
}

loader::action_t client_t::update() {
	for (;;) {
		static BYTE buf[65536];

		addr_t addr;
		i32 len = socket.recv(buf, sizeof(buf), &addr);
		if (len < 0) {
			// Would block.
			break;
		}

		// Ignore messages that don't originiate from the server's IP:port.
		// We can still received messages with spoofed IPs because this is UDP, however.
		if (addr.value != host.value) {
			continue;
		}

		// Ignore messages that don't have a header.
		if (len < sizeof(msg::header_t)) {
			continue;
		}

		// Verify the header version.
		auto header = (msg::header_t*)buf;
		if (header->version != msg::VERSION) {
			continue;
		}

		// Dispatch based on message type.
		switch (header->type) {
			case msg::TYPE_SERVER_UPDATE_TOKEN:
				updater.handle_msg_token(buf, len);
				break;

			case msg::TYPE_SERVER_UPDATE_BEGIN:
				updater.handle_msg_begin(buf, len);
				break;

			case msg::TYPE_SERVER_UPDATE_PIECE:
				updater.handle_msg_piece(buf, len);
				break;
		}
	}

	// Process updates.
	auto now_ms = timer::query_ms();
	updater.update(now_ms, host);

	// Test output.
	auto status = updater.get_status();

	static u32 last_bytes;
	if (last_bytes != status.bytes) {
		last_bytes = status.bytes;
		printf("debug: status %d progress: %u / %u\n", status.code, status.bytes, status.total);
	}

	bool restart = updater.needs_restart();
	if (restart) {
		printf("debug: update completed\n");
	}

	// Limit the update rate.
	timer::sleep_ms(10);

	return restart ? loader::action_t::reload : loader::action_t::none;
}

}
