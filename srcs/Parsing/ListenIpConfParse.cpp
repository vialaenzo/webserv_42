#include "ListenIpConfParse.hpp"

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "Log.hpp"
#include "usefull.hpp"

ListenIpConfParse::ListenIpConfParse() {}

ListenIpConfParse::ListenIpConfParse(std::string token) {
	std::vector<std::string> ip = ft_split(token, ':');

	if (ip.size() == 1) {
		if (ip[0].find(".") != std::string::npos) {
			_ip = ip[0];
			_port = 80;
		} else {
			_ip = "0.0.0.0";
			_port = std::atoi(ip[0].c_str());
		}
	} else if (ip.size() == 2) {
		if (ip[0].empty() || ip[1].empty()) {
			Log::log(Log::FATAL, "Invalid Ip value: %s", token.c_str());
			exit(Log::FATAL);
		}
		_ip = ip[0];
		_port = std::atoi(ip[1].c_str());
	} else {
		Log::log(Log::FATAL, "Invalid listen value: %s", token.c_str());
		exit(Log::FATAL);
	}
	_ipAndPort = _ip + ":" + UIntToString(_port);
	if (!checkIpPort()) {
		Log::log(Log::FATAL, "Invalid Ip/Port value: %s", token.c_str());
		exit(Log::FATAL);
	}
}

ListenIpConfParse::~ListenIpConfParse() {}

bool ListenIpConfParse::isValidIp(const std::string& ipStr) {
	std::vector<std::string> ip = ft_split(ipStr, '.');

	// Vérification qu'on a bien 4 octets
	if (ip.size() != 4) return false;

	for (size_t i = 0; i < ip.size(); i++) {
		// Vérification que chaque segment est un nombre et non vide
		if (ip[i].empty() || ip[i].size() > 3) return false;

		// Remplacement de std::all_of par une boucle classique
		for (size_t j = 0; j < ip[i].size(); j++) {
			if (!std::isdigit(ip[i][j])) return false;
		}

		// Vérification de la plage 0-255
		int segment = std::atoi(ip[i].c_str());
		if (segment < 0 || segment > 255) return false;
	}

	return true;
}

bool ListenIpConfParse::checkIpPort() {
	std::vector<std::string> ip_port = ft_split(_ipAndPort, ':');

	// Vérification qu'on a bien soit une IP seule, soit une IP avec un port
	if (ip_port.size() < 1 || ip_port.size() > 2) return false;

	// Vérification de l'IP avec la nouvelle fonction
	if (!isValidIp(ip_port[0])) return false;

	// Si un port est fourni, le vérifier
	if (ip_port.size() == 2) {
		if (ip_port[1].empty()) return false;

		// Remplacement de std::all_of par une boucle classique
		for (size_t j = 0; j < ip_port[1].size(); j++) {
			if (!std::isdigit(ip_port[1][j])) return false;
		}

		int port = std::atoi(ip_port[1].c_str());
		if (port < 0 || port > 65535) return false;
	}

	return true;
}
