#include "server_iptable.h"
#include "core_hash.h"
#include "core_util.h"
#include <string.h>

namespace core {

iptable_t::iptable_t() {
	reset();
}

void iptable_t::reset() {
	num = 0;
	memset(table, 0, sizeof(table));
}

void iptable_t::insert(u32 host, i64 now_ms) {
	auto slot = hash::mix32(host) % util::count(table);
	auto node = table[slot];

	while (node) {
		if (node->host == host) {
			break;
		}

		node = node->next;
	}

	if (node == nullptr) {
		if (num < util::count(data)) {
			node = &data[num++];
		} else {
			for (auto& iter : data) {
				if (now_ms - iter.time_ms > timeout_ms) {
					node = &iter;
					break;
				}
			}

			if (node == nullptr) {
				return;
			}

			remove(node->host);
		}

		node->host = host;
		node->count = 1;
		node->time_ms = now_ms;
		node->next = table[slot];
		table[slot] = node;

		return;
	}

	if (now_ms - node->time_ms > timeout_ms) {
		node->count = 1;
		node->time_ms = now_ms;
	} else {
		node->count += 1;
	}
}

void iptable_t::remove(u32 host) {
	auto slot = hash::mix32(host) % util::count(table);
	node_t* node = table[slot];
	node_t* prev = nullptr;

	while (node) {
		if (node->host == host) {
			if (prev) {
				prev->next = node->next;
			} else {
				table[slot] = node->next;
			}

			return;
		}

		prev = node;
		node = node->next;
	}
}

bool iptable_t::blocked(u32 host) {
	auto node = table[hash::mix32(host) % util::count(table)];
	while (node) {
		if (node->host == host) {
			return node->count >= max_connections_per_ip;
		}

		node = node->next;
	}

	return false;
}

}