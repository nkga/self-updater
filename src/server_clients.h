#pragma once
#include "core.h"
#include "core_msg.h"
#include "core_socket.h"

namespace core {

class iptable_t;
class crypto_t;

class clients_t {
public:
	class node_t {
	public:
		node_t* next;
		addr_t addr;
		u64 server_token;
		i64 time_ms;
		u32 bandwidth;

		// Updates current bandwidth limits. Returns true if node has bandwidth available to it.
		bool is_bandwidth_available(i64 now_ms);

	protected:
		// Rate to decrease bandwidth usage, bytes/ms.
		// Best set to a fraction of your maximum expected speed and to increase max_bandwidth instead.
		static constexpr i64 decrease_rate = 100;

		// Maximum expected bandwidth to transfer, bytes.
		static constexpr u32 max_bandwidth = 2000000;
	};

	// Initializes the clients table.
	clients_t(crypto_t* crypto, iptable_t* iptable);

	// Resets the clients table.
	void reset();

	// Inserts or returns the client for the given address.
	node_t* insert(addr_t addr, i64 now_ms);

	// Removes the client associated with the given address.
	void remove(addr_t addr);

protected:
	crypto_t * crypto;
	iptable_t* iptable;
	u32 num;
	node_t data[65536];
	node_t* table[32768];
};

}
