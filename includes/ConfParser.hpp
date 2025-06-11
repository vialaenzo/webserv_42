#ifndef CONFPARSER_HPP
#define CONFPARSER_HPP

#include <map>
#include <vector>

#include "BlockServer.hpp"

class BlockServer;

typedef std::map<std::string, std::vector<BlockServer> > MapServers;

class ConfParser {
public:
	static int countLineFile;

	ConfParser(void);
	~ConfParser(void);

	void parsing(const std::string &filename);

	/* METHODS */
	bool checkDoubleServerName();
	bool BlockServerBegin(std::vector<std::string> tokens);
	void ServersListens();

	/* CHERCKERS */
	static std::vector<std::string> supportedMethods;
	static bool CheckerMethod(std::string method);
	static std::vector<std::string> supportedHttpVersions;
	static bool CheckerHttpVersion(std::string version);

	/* GETTERS */
	MapServers getConfigs(void) const { return _configs; }
	MapServers &getServers(void) { return _configs; }

	void printServers();

private:
	std::string _filename;

	std::vector<BlockServer> _servers;
	MapServers _configs;

	/* GETTERS */
	static std::vector<std::string> _getSupportedMethods(void);
	static std::vector<std::string> _getSupportedHttpVersions(void);
};

#endif
