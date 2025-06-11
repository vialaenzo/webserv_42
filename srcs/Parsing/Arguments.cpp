#include "Arguments.hpp"

#include "Log.hpp"

bool check_arguments(int argc, char **argv) {
	if (argc <= 2) {
		if ((argc == 2 && std::string(argv[1]) == "--help") || argc == 1) {
			Log::log(
				Log::INFO,
				"Usage: \n./webserv [filename.conf] [options] \n \t --debug : "
				"Enable debug mode \n \t --save : Save log in file");
			return (false);
		} else
			return (filename_parser(argv[1]));
	} else if (argc > 2 && argc <= 4)
		return (check_options(argc, argv) && filename_parser(argv[1]));
	else {
		Log::log(Log::FATAL, "Bad numbers arguments");
		return (false);
	}
}

bool filename_parser(const std::string &filename) {
	if (filename.substr(filename.size() - 5) != ".conf") {
		Log::log(Log::FATAL, "Invalid extension");
		return false;
	}
	if (filename.size() < 6) {
		Log::log(Log::FATAL, "Invalid filename");
		return false;
	}
	return true;
}

bool check_options(int argc, char **argv) {
	for (int i = 2; i < argc; i++) {
		if (std::string(argv[i]) == "--debug")
			Log::setLogDebugState(true);
		else if (std::string(argv[i]) == "--save")
			Log::setLogFileState(true);
		else {
			Log::log(Log::FATAL, "Option : %s invalid", argv[i]);
			return false;
		}
	}
	return (true);
}
