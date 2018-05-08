#pragma once
#include "core.h"

namespace core {

class iptable_t {
public:
	struct node_t {
		node_t* next;
		u32 host;
		u32 count;
		i64 time_ms;
	};

	// Initializes the table.
	iptable_t();

	// Resets the data for the table.
	void reset();

	// Inserts a host into the table.
	void insert(u32 host, i64 now_ms);

	// Removes a host from the table.
	void remove(u32 host);

	// Returns true if the host should be blocked from connecting.
	bool blocked(u32 host);

protected:
	// Timeout value.
	static constexpr u32 const timeout_ms = 300000;

	// Maximum number of connections per IP before the time-out period.
	static constexpr u32 const max_connections_per_ip = 16;

	u32 num;
	node_t data[65536];
	node_t* table[16384];
};

}
