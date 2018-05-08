#include "core_socket.h"
#include <WS2tcpip.h>
#include <assert.h>

namespace core {


socket_t::socket_t() : hsocket(invalid), is_wsa_initialized(false) {
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		return;
	}

	is_wsa_initialized = true;
}

socket_t::~socket_t() {
	close();

	if (is_wsa_initialized) {
		WSACleanup();
	}
}

bool socket_t::open() {
	close();
	hsocket = socket(AF_INET, SOCK_DGRAM, 0);
	return (hsocket != invalid);
}

void socket_t::close() {
	if (hsocket != invalid) {
		closesocket(hsocket);
		hsocket = invalid;
	}
}

bool socket_t::bind(u16 port) {
	struct sockaddr_in si = {};
	si.sin_family = AF_INET;
	si.sin_addr.s_addr = INADDR_ANY;
	si.sin_port = htons(port);

	return (::bind(hsocket, (struct sockaddr*)&si, sizeof(si)) == 0);
}

i32 socket_t::recv(void* dst, i32 dst_bytes, addr_t* addr) {
	assert(dst);
	assert(dst_bytes);
	assert(addr);

	if (hsocket == invalid) {
		return -1;
	}

	struct sockaddr_in si = {};
	i32 sil = sizeof(si);
	i32 len = recvfrom(hsocket, (char*)dst, dst_bytes, 0, (struct sockaddr*)&si, &sil);

	addr->host = si.sin_addr.s_addr;
	addr->port = si.sin_port;

	return len;
}

i32 socket_t::send(void const* src, i32 src_bytes, addr_t addr) {
	assert(src);
	assert(src_bytes);

	if (hsocket == invalid) {
		return -1;
	}

	if (addr.value == 0) {
		return 0;
	}

	SOCKADDR_IN si = {};
	si.sin_family = AF_INET;
	si.sin_addr.s_addr = addr.host;
	si.sin_port = (u16)addr.port;

	return sendto(hsocket, (CHAR const*)src, src_bytes, 0, (SOCKADDR*)&si, sizeof(si));
}

bool socket_t::set_non_blocking(bool enabled) {
	if (hsocket == invalid) {
		return false;
	}

	u_long val = enabled;
	return (ioctlsocket(hsocket, FIONBIO, &val) == 0);
}

addr_t socket_t::lookup(CHAR const* host, u16 port) {
	assert(host);

	addr_t addr = {};

	struct addrinfo hints = {};
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	struct addrinfo* res = nullptr;
	if (getaddrinfo(host, 0, &hints, &res) == 0) {
		auto sai = (struct sockaddr_in*)res->ai_addr;
		addr.host = sai->sin_addr.s_addr;
		addr.port = htons(port);
		freeaddrinfo(res);
	}

	return addr;
}
}

