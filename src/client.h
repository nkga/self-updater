#pragma once
#include "core_crypto.h"
#include "core_socket.h"
#include "client_updater.h"
#include "loader.h"

namespace core {

class client_t {
public:
	// Creates the client.
	client_t();

	// Destroys the client.
	~client_t();

	// Updates the client.
	loader::action_t update();

protected:
	crypto_t crypto;
	socket_t socket;
	updater_t updater;
	addr_t host;
};

}
