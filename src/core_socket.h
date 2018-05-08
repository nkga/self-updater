#pragma once
#include "core.h"
#include <WinSock2.h>

namespace core {

// IPV4 host port/pair.
union addr_t {
	u64 value;

	struct {
		u32 host;
		u32 port;
	};
};

// Thin wrapper around a UDP socket.
class socket_t {
public:
	// Initializes the socket.
	socket_t();

	// Destroys the socket.
	~socket_t();

	// Opens the socket.
	bool open();

	// Closes the socket.
	void close();

	// Binds the socket to the given port. If the port is 0, the port is automatically picked.
	bool bind(u16 port);

	// Receives a message from the network. Returns the number of bytes read on success and a negative value on failure.
	i32 recv(void* dst, i32 dst_bytes, addr_t* addr);

	// Sends a message to the given address. Returns the number of bytes sent on success and a negative value on failure.
	i32 send(void const* src, i32 src_bytes, addr_t addr);

	// Sets the blocking mode for the socket.
	bool set_non_blocking(bool enabled);

	// Returns the address for a given host:port pair.
	static addr_t lookup(CHAR const* host, u16 port);

protected:
	// Invalid socket value.
	static constexpr SOCKET invalid = INVALID_SOCKET;

	SOCKET hsocket;
	bool is_wsa_initialized;
};

}
