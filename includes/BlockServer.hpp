#ifndef BLOCKSERVER_HPP
#define BLOCKSERVER_HPP

#include "BlockLocation.hpp"
#include "ListenIpConfParse.hpp"

class Log;
class ConfParser;
class BlockLocation;
class ListenIpConfParse;
class Usefull;

#define DF_CLIENT_MAX_BODY 10485760	 // 10MO
class BlockServer {
private:
	std::map<std::string, ListenIpConfParse> _listens;
	std::vector<std::string> _serverNames;
	std::vector<std::string> _indexes;
	std::string _root;
	unsigned long long _clientMaxBodySize;
	std::vector<BlockLocation> _locations;
	std::map<int, std::string> _errorPages;
	std::string _uploadPath;

	// divers
	std::string _filename;
	std::map<std::string, int> _counterBase;

	// Methods
	bool DoubleLineChecker();
	void incrementCounter(const std::string &key) { _counterBase[key]++; }
	bool ValidServerChecker(std::vector<std::string> &tokens, std::string &key,
							std::ifstream &configFile);
	bool isStartBlockLocation(std::vector<std::string> &tokens);
	bool DoubleLocationChecker();
	void cleanPaths();

public:
	BlockServer(std::string filename);
	BlockServer(void);
	BlockServer(const BlockServer &other);
	~BlockServer(void);

	BlockServer &operator=(const BlockServer &other);

	// parsing
	BlockServer getServerConfig(std::ifstream &file_config);
	bool VerifEmptyRRI();

	// Getters
	BlockLocation *getLocationByPath(const std::string &path);
	const std::map<int, std::string> &getErrorPages() const {
		return _errorPages;
	}
	const std::vector<std::string> &getServerNames() const {
		return _serverNames;
	}
	std::vector<BlockLocation> *getLocations() { return &_locations; }
	const std::string &getRoot() const { return _root; }
	unsigned long long getClientMaxBodySize() const {
		return _clientMaxBodySize;
	}
	const std::map<std::string, ListenIpConfParse> &getListens() const {
		return _listens;
	}
	const std::vector<std::string> &getIndexes() const { return _indexes; }
	std::string getUploadPath() { return _uploadPath; }

	//	// Util
	bool isServerNamePresent(std::vector<std::string> &otherNames);
	bool VerifUploadPath(const std::string &uploadpath);

	//// Setters
	void setClientMaxBodySize(std::string clientMaxBodySize);
	void setRoot(const std::string &root);
	void setDefaultValue();
	void setLocations(const std::vector<BlockLocation> &locations) {
		_locations = locations;
	}
	void setErrorPages(const std::map<int, std::string> &errorPage) {
		_errorPages = errorPage;
	}
	void setUploadPath(std::string uploadpath) { _uploadPath = uploadpath; };

	//// Adders
	void addErrorPages(int errorCode, std::string file);
	void addLocation(const BlockLocation &locations) {
		_locations.push_back(locations);
	}
	void addListen(std::string &token);
	void addServerName(std::vector<std::string> &token);
	void addIndexes(std::vector<std::string> &token);

	// Finders
	BlockLocation *LocationPositionChecker(const std::string &part);

	// Printers
	void printServer(void);
	void printListens();
	void printPair(const std::string &label, const std::string &value);
	void printInt(const std::string &label, int value);
	void printVector(const std::string &label,
					 const std::vector<std::string> &vec);
	void printMap(const std::string &label,
				  const std::map<int, std::string> &map);
};

#endif
