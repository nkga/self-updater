#include "server.h"

int main(int argc, char** argv) {
	core::server_t* server = new core::server_t();
	server->run();
	delete server;
	return 0;
}
