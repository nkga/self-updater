#include "server_clients.h"
#include "server_iptable.h"
#include "core_crypto.h"
#include "core_hash.h"
#include "core_util.h"
#include <string.h>
#include <stdio.h>

namespace core {

clients_t::clients_t(crypto_t* crypto, iptable_t* iptable) {
	this->crypto = crypto;
	this->iptable = iptable;
	reset();
}

void clients_t::reset() {
	num = 0;
	memset(table, 0, sizeof(table));
}

clients_t::node_t* clients_t::insert(addr_t addr, i64 now_ms) {
	auto slot = core::hash::mix64(addr.value) % util::count(table);
	auto node = table[slot];

	while (node) {
		if (node->addr.value == addr.value) {
			break;
		}

		node = node->next;
	}

	if (node == nullptr) {
		if (iptable->blocked(addr.host)) {
			return nullptr;
		}

		if (num < util::count(data)) {
			node = &data[num++];
		} else {
			for (auto& iter : data) {
				if (now_ms - iter.time_ms > 30000) {
					node = &iter;
					break;
				}
			}

			if (node == nullptr) {
				return nullptr;
			}

			remove(addr);
		}

		memset(node, 0, sizeof(*node));
		node->addr = addr;
		node->server_token = crypto->gen_token();
		node->time_ms = now_ms;
		node->next = table[slot];
		table[slot] = node;

		iptable->insert(addr.host, now_ms);
	}

	return node;
}

void clients_t::remove(addr_t addr) {
	auto slot = hash::mix64(addr.value) % util::count(table);
	node_t* node = table[slot];
	node_t* prev = nullptr;

	while (node) {
		if (node->addr.value == addr.value) {
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

bool clients_t::node_t::is_bandwidth_available(i64 now_ms) {
	i64 decrease = (now_ms - time_ms);
	time_ms = now_ms;

	if (decrease > 0) {
		decrease *= decrease_rate; // bytes/ms

		if ((i64)bandwidth > decrease) {
			bandwidth -= (u32)decrease;
		} else {
			bandwidth = 0;
		}
	}

	return (bandwidth < max_bandwidth);
}

}
