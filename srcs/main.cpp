#include <cstdlib>

#include "Arguments.hpp"
#include "ConfParser.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "usefull.hpp"

Server* g_server;

int ConfParser::countLineFile = 0;

int main(int argc, char** argv, char** env) {
	(void)env;
	Server server;
	g_server = &server;

	if (!check_arguments(argc, argv)) return 1;

	signal(SIGINT, handle_signal);
	try {
		server.getParser().parsing(argv[1]);
		Log::log(Log::DEBUG, "Parsing completed");
		if (Log::getLogDebugState() == true) server.getParser().printServers();
		server.init();
		server.execute();
	} catch (std::exception& e) {
		return (EXIT_FAILURE);
	}
	Log::log(Log::DEBUG, "Server stopped.");
	return (EXIT_SUCCESS);
}
