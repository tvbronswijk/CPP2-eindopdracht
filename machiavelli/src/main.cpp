#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>  
#include <crtdbg.h>
#endif

#include "machiavelli/Game.hpp"
#include <server.hpp>

void run_game() 
{
	auto game = std::make_unique<Game>();
	auto server = create_server()
		.with_name("Machiavelli")
		.with_prompt("machiavelli> ")
		.at_port(1080)
		.build(std::move(game));

	server->accept([&server](Socket sock) {
		(*server) << "A new connection from " << sock.get_dotted_ip() << " has been established.\n";
		server->add_client(std::move(sock));
	});

	close_server(*server);
}

int main(int argc, const char * argv[])
{
#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	run_game();
	return 0;
}


