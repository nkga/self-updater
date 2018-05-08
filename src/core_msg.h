#pragma once
#include "core.h"

namespace core {
namespace msg {

// The address of the update server.
inline const CHAR net_host[] = "127.0.0.1";

// The port of the update server.
constexpr u16 net_port = 33333;

// Protocol version.
enum { VERSION = 102 };

// Message types.
enum type_t {
	TYPE_NONE,
	TYPE_CLIENT_UPDATE_BEGIN,
	TYPE_CLIENT_UPDATE_PIECE,
	TYPE_SERVER_UPDATE_TOKEN,
	TYPE_SERVER_UPDATE_BEGIN,
	TYPE_SERVER_UPDATE_PIECE,
};

// Frequency for token requests, milliseconds.
constexpr i64 freq_client_update_token_ms = 500;

// Frequency for update checks, milliseconds.
constexpr i64 freq_client_update_begin_ms = 1000;

// Frequency for piece requests, milliseconds.
constexpr i64 freq_client_update_piece_ms = 100;

// Size of the cryptographic signature, in bytes.
enum { SIG_BYTES = 512 };

// Size of a piece, in bytes.
enum { PIECE_BYTES = 1024 };

// Crytographic signature.
struct sig_t {
	BYTE data[SIG_BYTES];
};

// Messages are tightly packed.
#pragma pack(push, 1)

// Message header.
struct header_t {
	u16 version;
	u16 type;
};

// Requests a connection with the server and new update data.
struct client_update_begin_t {
	header_t header;
	u64 client_token;
	u64 server_token;
	u32 client_version;
};

// Requests a piece of the update data from the server.
struct client_update_piece_t {
	header_t header;
	u64 client_token;
	u64 server_token;
	u32 server_version;
	u32 piece_pos;
};

// Update metadata.
struct server_update_begin_t {
	header_t header;
	u64 client_token;
	u64 server_token;
	u32 server_version;
	u32 update_size;
	sig_t update_sig;
};

// Update token.
struct server_update_token_t {
	header_t header;
	u64 client_token;
	u64 server_token;
};

// A piece of the update data.
struct server_update_piece_t {
	header_t header;
	u64 client_token;
	u64 server_token;
	u32 server_version;
	u32 piece_pos;
	u16 piece_size;
};

#pragma pack(pop)

}
}
