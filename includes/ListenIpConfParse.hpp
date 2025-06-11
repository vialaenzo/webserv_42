#ifndef LISTENIPCONFPARSE_HPP
#define LISTENIPCONFPARSE_HPP

#include <string>

class ListenIpConfParse {
private:
	std::string _ip;
	unsigned int _port;
	std::string _ipAndPort;

	// Checker
	bool checkIpPort();
	bool isValidIp(const std::string& ipStr);

public:
	ListenIpConfParse();
	ListenIpConfParse(std::string token);
	~ListenIpConfParse();

	// Getters
	const std::string& getIp() const { return _ip; }
	unsigned int getPort() const { return _port; }
	const std::string getIpPortJoin() const { return _ipAndPort; }
};

#endif
