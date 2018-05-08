#include "server.h"
#include "core_filesystem.h"
#include "core_hash.h"
#include "core_heap.h"
#include "core_msg.h"
#include "core_timer.h"
#include "core_util.h"
#include <stdio.h>
#include <time.h>

namespace core {

using namespace msg;

WCHAR const server_t::update_file_path[] = L"update.dll";
WCHAR const server_t::update_sig_path[] = L"update.sig";

server_t::server_t() : clients(&crypto, &iptable) {
	last_update_check_ms = 0;
	last_update_write_ms = 0;
	update_version = 0;
	update_sig = {};
}

server_t::~server_t() {
}

void server_t::run() {
	printf("build %s version %u server port: %u\n\n", __TIMESTAMP__, VERSION, net_port);

	for (;;) {
		socket.open();

		if (socket.bind(net_port) == false) {
			printf("error: couldn't bind socket.\n");
			break;
		}

		for (;;) {
			if (step() == false) {
				break;
			}
		}

		printf("error: network interface failure.\n");
		socket.close();

		timer::sleep_ms(5000);
	}

	printf("error: socket creation failed.\n");
}

bool server_t::step() {
	static BYTE buf[65536];

	addr_t addr;
	i32 len = socket.recv(buf, sizeof(buf), &addr);
	if (len < 0) {
		return false;
	}

	if (len < sizeof(header_t)) {
		return true;
	}

	auto header = (header_t*)buf;
	if (header->version != VERSION) {
		return true;
	}

	if (header->type == TYPE_CLIENT_UPDATE_BEGIN) {
		msg_client_update_begin(buf, len, addr);
	} else if (header->type == TYPE_CLIENT_UPDATE_PIECE) {
		msg_client_update_piece(buf, len, addr);
	}

	return true;
}

void server_t::msg_client_update_begin(BYTE* buf, u32 len, addr_t addr) {
	if (len != sizeof(client_update_begin_t)) {
		return;
	}

	auto msg = (client_update_begin_t*)buf;

	// You probably want to check that client_version isn't 0 here, but we use that for demoing.

	auto now_ms = timer::query_ms();
	auto client = clients.insert(addr, now_ms);

	if (client == nullptr) {
		return;
	}

	if (client->is_bandwidth_available(now_ms) == false) {
		return;
	}

	if (msg->server_token != client->server_token) {
		server_update_token_t res = {};
		res.header.type = TYPE_SERVER_UPDATE_TOKEN;
		res.header.version = VERSION;
		res.client_token = msg->client_token;
		res.server_token = client->server_token;
		socket.send(&res, sizeof(res), addr);

		client->bandwidth += sizeof(server_update_token_t);

		return;
	}

	if (update_refresh(now_ms) == false) {
		return;
	}

	printf("debug: update  (%d.%d.%d.%d:%d) [%08X] -> [%08X]\n",
		(addr.host & 0xFF),
		(addr.host >> 8) & 0xFF,
		(addr.host >> 16) & 0xFF,
		(addr.host >> 24) & 0xFF,
		addr.port,
		msg->client_version,
		update_version);

	if (msg->client_version == update_version) {
		return;
	}

	server_update_begin_t res;
	memset(&res, 0, sizeof(res));

	res.header.version = VERSION;
	res.header.type = TYPE_SERVER_UPDATE_BEGIN;
	res.client_token = msg->client_token;
	res.server_token = client->server_token;
	res.server_version = update_version;
	res.update_size = update_file.size;
	res.update_sig = update_sig;

	socket.send(&res, sizeof(res), addr);

	client->bandwidth += sizeof(server_update_begin_t);
}

void server_t::msg_client_update_piece(BYTE * buf, u32 len, addr_t addr) {
	if (len != sizeof(msg::client_update_piece_t)) {
		return;
	}

	auto now_ms = timer::query_ms();

	if (update_refresh(now_ms) == false) {
		return;
	}

	auto msg = (msg::client_update_piece_t*)buf;

	if (msg->server_version != update_version) {
		return;
	}

	if (msg->piece_pos >= update_file.size) {
		return;
	}

	auto client = clients.insert(addr, now_ms);

	if (client == nullptr) {
		return;
	}

	if (client->is_bandwidth_available(now_ms) == false) {
		return;
	}

	if (msg->server_token != client->server_token) {
		return;
	}

	msg::server_update_piece_t res = {};
	res.header.version = msg::VERSION;
	res.header.type = msg::TYPE_SERVER_UPDATE_PIECE;
	res.client_token = msg->client_token;
	res.server_token = client->server_token;
	res.server_version = update_version;
	res.piece_pos = msg->piece_pos;
	res.piece_size = (u16)util::min<u32>(update_file.size - msg->piece_pos, PIECE_BYTES);
	
	CHAR send_buf[sizeof(res) + PIECE_BYTES];
	memcpy(send_buf, &res, sizeof(res));
	memcpy(send_buf + sizeof(res), update_file.data + msg->piece_pos, res.piece_size);

	u32 send_bytes = sizeof(res) + res.piece_size;
	socket.send(send_buf, send_bytes, addr);
	client->bandwidth += send_bytes;
}

bool server_t::update_refresh(i64 now_ms) {
	if (now_ms - last_update_check_ms > 15000){
		last_update_check_ms = now_ms;

		i64 file_time_ms = core::filesystem::get_write_ms(update_sig_path);
		if (file_time_ms != last_update_write_ms) {
			last_update_write_ms = file_time_ms;

			update_file.free();
			if (filesystem::read(&update_file, update_file_path)) {
				update_version = hash::crc32(update_file.data, update_file.size);
			}

			if (filesystem::file_t file; file.open(GENERIC_READ, OPEN_EXISTING, update_sig_path)) {
				file.read(&update_sig, sizeof(update_sig));
			}
		}
	}

	return (update_file.data != nullptr);
}

}
