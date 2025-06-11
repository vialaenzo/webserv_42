#include "ConfParser.hpp"

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>

#include "Log.hpp"

std::vector<std::string> ConfParser::supportedMethods =
	ConfParser::_getSupportedMethods();

std::vector<std::string> ConfParser::supportedHttpVersions =
	ConfParser::_getSupportedHttpVersions();

ConfParser::ConfParser(void) {}

ConfParser::~ConfParser(void) {}

/*__  __      _   _               _
 |  \/  |    | | | |             | |
 | \  / | ___| |_| |__   ___   __| |___
 | |\/| |/ _ \ __| '_ \ / _ \ / _` / __|
 | |  | |  __/ |_| | | | (_) | (_| \__ \
 |_|  |_|\___|\__|_| |_|\___/ \__,_|___/

										*/

std::vector<std::string> ConfParser::_getSupportedMethods(void) {
	std::vector<std::string> methods;

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	return (methods);
}

bool ConfParser::CheckerMethod(std::string method) {
	return (std::find(ConfParser::supportedMethods.begin(),
					  ConfParser::supportedMethods.end(),
					  method) != ConfParser::supportedMethods.end());
}

/*_    _ _______ _______ _____
 | |  | |__   __|__   __|  __ \
 | |__| |  | |     | |  | |__) |
 |  __  |  | |     | |  |  ___/
 | |  | |  | |     | |  | |
 |_|  |_|  |_|     |_|  |_|

								*/

std::vector<std::string> ConfParser::_getSupportedHttpVersions(void) {
	std::vector<std::string> versions;

	versions.push_back("HTTP/1.0");
	versions.push_back("HTTP/1.1");

	return (versions);
}

bool ConfParser::CheckerHttpVersion(std::string method) {
	return (std::find(ConfParser::supportedHttpVersions.begin(),
					  ConfParser::supportedHttpVersions.end(),
					  method) != ConfParser::supportedHttpVersions.end());
}

void ConfParser::ServersListens() {
	for (size_t i = 0; i < _servers.size(); i++) {
		std::map<std::string, ListenIpConfParse> listens =
			_servers[i].getListens();
		for (std::map<std::string, ListenIpConfParse>::iterator it =
				 listens.begin();
			 it != listens.end(); ++it) {
			_configs[it->first].push_back(_servers[i]);
		}
	}
}

/*_____  _____  _____ _   _ _______ ______ _____   _____
 |  __ \|  __ \|_   _| \ | |__   __|  ____|  __ \ / ____|
 | |__) | |__) | | | |  \| |  | |  | |__  | |__) | (___
 |  ___/|  _  /  | | | . ` |  | |  |  __| |  _  / \___ \
 | |    | | \ \ _| |_| |\  |  | |  | |____| | \ \ ____) |
 |_|    |_|  \_\_____|_| \_|  |_|  |______|_|  \_\_____/

														  */

void ConfParser::printServers(void) {
	for (size_t i = 0; i < _servers.size(); i++) {
		std::cout << "============ SERVER " << i + 1 << " ===========\n"
				  << std::endl;
		_servers[i].printServer();
		std::cout << std::endl;
	}
}

/* CHECKERS Servers */

bool ConfParser::checkDoubleServerName() {
	std::vector<std::string> serverNames;

	for (size_t i = 0; i < _servers.size(); i++) {
		std::vector<std::string> currNames = _servers[i].getServerNames();
		for (size_t j = i + 1; j < _servers.size(); j++) {
			if (_servers[j].isServerNamePresent(currNames)) {
				Log::log(Log::FATAL, "conflicting server name \"%s\" on %s",
						 currNames[0].c_str(), currNames[1].c_str());
				return false;
			}
		}
	}
	return true;
}

bool ConfParser::BlockServerBegin(std::vector<std::string> tokens) {
	return ((tokens.size() == 2 && tokens[0] == "server" &&
			 tokens[1] == "{")) ||
		   (tokens.size() == 1 && tokens[0] == "server{");
}

/*_____        _____   _____ _____ _   _  _____
 |  __ \ /\   |  __ \ / ____|_   _| \ | |/ ____|
 | |__) /  \  | |__) | (___   | | |  \| | |  __
 |  ___/ /\ \ |  _  / \___ \  | | | . ` | | |_ |
 | |  / ____ \| | \ \ ____) |_| |_| |\  | |__| |
 |_| /_/    \_\_|  \_\_____/|_____|_| \_|\_____|

												 */

void ConfParser::parsing(const std::string &filename) {
	_filename = filename;
	Log::log(Log::DEBUG, "Parsing config file: %s", _filename.c_str());

	std::ifstream configFile(_filename.c_str());
	if (!configFile.is_open()) {
		Log::log(Log::FATAL, "File %s can't be opened or doesn't exist",
				 _filename.c_str());
		exit(Log::FATAL);
	}

	std::string line;
	while (std::getline(configFile, line)) {
		++countLineFile;
		line = trim(line, " \t\n\r\f\v");

		if (line.empty() || line[0] == '#') {
			continue;
		}

		std::vector<std::string> tokens = ft_split(line, ' ');
		if (BlockServerBegin(tokens)) {
			BlockServer server(_filename);
			_servers.push_back(server.getServerConfig(configFile));
		} else {
			Log::log(Log::FATAL, "Invalid line: \"%s\" in file: %s:%d",
					 line.c_str(), _filename.c_str(), countLineFile);
			exit(Log::FATAL);
		}
	}

	if (_servers.empty()) {
		BlockServer server(_filename);
		_servers.push_back(server.getServerConfig(configFile));
	}

	if (!checkDoubleServerName()) exit(Log::FATAL);
	ServersListens();
	configFile.close();
}
