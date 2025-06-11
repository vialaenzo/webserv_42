#include "BlockServer.hpp"

#include <sys/stat.h>

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>

#include "ConfParser.hpp"
#include "Log.hpp"

BlockServer::BlockServer(void)
	: _clientMaxBodySize(DF_CLIENT_MAX_BODY), _filename("") {
	_counterBase["root"] = 0;
	_counterBase["clientMaxBodySize"] = 0;
	_uploadPath = "./goodweb/main/cgi-bin/uploads";
}

BlockServer::BlockServer(std::string filename)
	: _clientMaxBodySize(DF_CLIENT_MAX_BODY), _filename(filename) {
	_counterBase["root"] = 0;
	_counterBase["clientMaxBodySize"] = 0;
	_uploadPath = "./goodweb/main/cgi-bin/uploads";
}

BlockServer::BlockServer(const BlockServer &other) { *this = other; }

BlockServer::~BlockServer(void) {}

BlockServer &BlockServer::operator=(const BlockServer &other) {
	if (this != &other) {
		_listens = other._listens;
		_serverNames = other._serverNames;
		_indexes = other._indexes;
		_root = other._root;
		_clientMaxBodySize = other._clientMaxBodySize;
		_locations = other._locations;
		_errorPages = other._errorPages;
		_filename = other._filename;
		_counterBase = other._counterBase;
		_uploadPath = other._uploadPath;
	}
	return *this;
}

/*_____      _            _
 |  __ \    (_)          | |
 | |__) | __ ___   ____ _| |_ ___
 |  ___/ '__| \ \ / / _` | __/ _ \
 | |   | |  | |\ V / (_| | ||  __/
 |_|   |_|  |_| \_/ \__,_|\__\___|

								   */

bool BlockServer::DoubleLineChecker() {
	std::map<std::string, int>::iterator it;

	for (it = _counterBase.begin(); it != _counterBase.end(); ++it)
		if (it->second > 1) {
			Log::log(Log::FATAL, "Duplicate line in server context: %s",
					 it->first.c_str());
			return false;
		}
	return true;
}

bool BlockServer::isStartBlockLocation(std::vector<std::string> &tokens) {
	return (tokens.size() == 3 && tokens[0] == "location" && tokens[2] == "{");
}

bool BlockServer::DoubleLocationChecker() {
	std::vector<BlockLocation>::iterator it;
	std::vector<BlockLocation>::iterator it2;

	for (it = _locations.begin(); it != _locations.end(); ++it) {
		for (it2 = it + 1; it2 != _locations.end(); ++it2) {
			if (it->getPath() == it2->getPath()) {
				Log::log(Log::FATAL, "Duplicate location: %s",
						 it->getPath().c_str());
				return false;
			}
		}
	}
	return true;
}

void BlockServer::cleanPaths() {
	if (!_root.empty() && _root != "/" && _root[_root.size() - 1] == '/')
		_root.erase(_root.size() - 1);

	for (std::map<int, std::string>::iterator it = _errorPages.begin();
		 it != _errorPages.end(); ++it) {
		if (it->second != "/" && it->second[it->second.size() - 1] == '/')
			it->second.erase(it->second.size() - 1);
	}

	std::vector<BlockLocation>::iterator it;
	for (it = _locations.begin(); it != _locations.end(); ++it) {
		it->cleanPaths();
	}
}

bool BlockServer::VerifUploadPath(const std::string &uploadpath) {
	if (!uploadpath.empty() && uploadpath != "/" &&
		uploadpath[uploadpath.size() - 1] == '/') {
		struct stat info;

		if (stat(uploadpath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
			return true;
		}
	}
	return false;
}

bool BlockServer::ValidServerChecker(std::vector<std::string> &tokens,
									 std::string &key,
									 std::ifstream &configFile) {
	if (tokens.size() < 2) return false;
	if (isStartBlockLocation(tokens)) {
		BlockLocation location(_filename);
		addLocation(location.getLocationConfig(configFile, tokens[1]));
	} else if (key == "listen" && tokens.size() == 2)
		addListen(tokens[1]);
	else if (key == "server_name")
		addServerName(tokens);
	else if (key == "index")
		addIndexes(tokens);
	else if (key == "root" && tokens.size() == 2)
		setRoot(tokens[1]);
	else if (key == "client_max_body_size" && tokens.size() == 2)
		setClientMaxBodySize(tokens[1]);
	else if (key == "error_page" && tokens.size() == 3)
		addErrorPages(std::atoi(tokens[1].c_str()), tokens[2]);
	else if (key == "upload_path" && tokens.size() == 2 &&
			 VerifUploadPath(tokens[1])) {
		setUploadPath(tokens[1]);
	} else
		return (false);
	return (true);
}

/* _____ ______ _______ _______ ______ _____   _____
  / ____|  ____|__   __|__   __|  ____|  __ \ / ____|
 | (___ | |__     | |     | |  | |__  | |__) | (___
  \___ \|  __|    | |     | |  |  __| |  _  / \___ \
  ____) | |____   | |     | |  | |____| | \ \ ____) |
 |_____/|______|  |_|     |_|  |______|_|  \_\_____/

													  */

void BlockServer::setClientMaxBodySize(std::string clientMaxBodySize) {
	if (clientMaxBodySize == "none")
		_clientMaxBodySize = 0;
	else {
		char *end;
		unsigned long long value =
			std::strtoull(clientMaxBodySize.c_str(), &end, 10);

		if (*end != '\0')
			throw std::invalid_argument("Invalid number format: " +
										clientMaxBodySize);

		_clientMaxBodySize = value;
	}
	_counterBase["clientMaxBodySize"]++;
}

void BlockServer::setRoot(const std::string &root) {
	_root = root;
	_counterBase["root"]++;
}

void BlockServer::setDefaultValue() {
	if (_listens.empty()) {
		ListenIpConfParse listen("0.0.0.0:1234");
		_listens["0.0.0.0:1234"] = listen;
	}
	if (_root.empty()) _root = "./goodweb/";
	if (_indexes.empty()) _indexes.push_back("index.html");
}

bool BlockServer::isServerNamePresent(std::vector<std::string> &otherNames) {
	// Utiliser un set pour une recherche rapide
	std::set<std::string> existingNames(_serverNames.begin(),
										_serverNames.end());

	for (std::vector<std::string>::const_iterator it = otherNames.begin();
		 it != otherNames.end(); ++it) {
		if (existingNames.find(*it) != existingNames.end()) {
			// Si un doublon est trouvé, modifier otherNames comme spécifié
			otherNames.clear();
			otherNames.push_back(*it);
			otherNames.push_back(_listens.begin()->first);
			return true;
		}
	}
	return false;
}

/*         _____  _____  ______ _____   _____
	 /\   |  __ \|  __ \|  ____|  __ \ / ____|
	/  \  | |  | | |  | | |__  | |__) | (___
   / /\ \ | |  | | |  | |  __| |  _  / \___ \
  / ____ \| |__| | |__| | |____| | \ \ ____) |
 /_/    \_\_____/|_____/|______|_|  \_\_____/

											   */

void BlockServer::addErrorPages(int errorCode, std::string file) {
	// Vérifie que le code d'erreur est valide (400-599)
	if (errorCode < 400 || errorCode > 599) {
		Log::log(Log::FATAL, "Invalid error code: %d in file %s:%d", errorCode,
				 _filename.c_str(), ConfParser::countLineFile);
	}

	// Vérifie qu'on n'ajoute pas un code d'erreur en double
	if (_errorPages.find(errorCode) != _errorPages.end()) {
		Log::log(Log::FATAL, "Duplicate error code: %d", errorCode);
	}

	// Ajoute le fichier d'erreur
	_errorPages[errorCode] = file;
}

void BlockServer::addListen(std::string &token) {
	ListenIpConfParse listen(token);
	const std::string &ipPort = listen.getIpPortJoin();

	if (_listens.find(ipPort) != _listens.end()) {
		Log::log(Log::FATAL, "Duplicate listen in server context: %s",
				 token.c_str());
		throw std::runtime_error("Duplicate listen: " + token);
	}

	_listens[ipPort] = listen;
}

void BlockServer::addIndexes(std::vector<std::string> &token) {
	std::set<std::string> existingIndexes(_indexes.begin(), _indexes.end());

	for (size_t i = 1; i < token.size(); i++) {
		{
			_indexes.push_back(token[i]);
			existingIndexes.insert(token[i]);
		}
	}
}

void BlockServer::addServerName(std::vector<std::string> &token) {
	std::set<std::string> existingNames(_serverNames.begin(),
										_serverNames.end());

	for (size_t i = 1; i < token.size(); i++) {
		if (existingNames.insert(token[i]).second)
			_serverNames.push_back(token[i]);
	}
}

/*_____  _____  _____ _   _ _______ ______ _____   _____
 |  __ \|  __ \|_   _| \ | |__   __|  ____|  __ \ / ____|
 | |__) | |__) | | | |  \| |  | |  | |__  | |__) | (___
 |  ___/|  _  /  | | | . ` |  | |  |  __| |  _  / \___ \
 | |    | | \ \ _| |_| |\  |  | |  | |____| | \ \ ____) |
 |_|    |_|  \_\_____|_| \_|  |_|  |______|_|  \_\_____/

														  */

void BlockServer::printVector(const std::string &label,
							  const std::vector<std::string> &vec) {
	std::cout << std::setw(25) << std::left << label << ": ";
	if (vec.empty())
		std::cout << "none" << std::endl;
	else {
		for (std::vector<std::string>::const_iterator it = vec.begin();
			 it != vec.end(); ++it)
			std::cout << *it << " ";
		std::cout << std::endl;
	}
}

void BlockServer::printListens() {
	std::cout << std::setw(25) << std::left << "Listens" << ": ";
	if (_listens.empty())
		std::cout << "none" << std::endl;
	else {
		for (std::map<std::string, ListenIpConfParse>::const_iterator it =
				 _listens.begin();
			 it != _listens.end(); ++it)
			std::cout << it->first << " ";
		std::cout << std::endl;
	}
}

void BlockServer::printPair(const std::string &label,
							const std::string &value) {
	std::cout << std::setw(25) << std::left << label << ": " << value
			  << std::endl;
}

void BlockServer::printInt(const std::string &label, int value) {
	std::cout << std::setw(25) << std::left << label << ": " << value
			  << std::endl;
}

void BlockServer::printMap(const std::string &label,
						   const std::map<int, std::string> &map) {
	std::cout << std::setw(25) << std::left << label << ":" << std::endl;
	if (map.empty())
		std::cout << "  none" << std::endl;
	else {
		for (std::map<int, std::string>::const_iterator it = map.begin();
			 it != map.end(); ++it)
			std::cout << "  " << it->first << ": " << it->second << std::endl;
	}
}

void BlockServer::printServer(void) {
	printVector("Server names", _serverNames);
	printListens();
	printVector("Indexes", _indexes);
	printPair("Upload Path", _uploadPath);
	printPair("Root", _root);
	std::cout << "Client max body size: " << ullToStr(_clientMaxBodySize)
			  << std::endl;
	printMap("Error pages", _errorPages);

	if (_locations.empty())
		std::cout << std::setw(25) << std::left << "Locations" << ": none"
				  << std::endl;
	else {
		int i = 0;
		for (std::vector<BlockLocation>::iterator it = _locations.begin();
			 it != _locations.end(); ++it) {
			std::cout << "\n-- LOCATION " << ++i << " --" << std::endl;
			it->printLocation();
		}
	}
}

/*______ _____ _   _ _____  ______ _____
 |  ____|_   _| \ | |  __ \|  ____|  __ \
 | |__    | | |  \| | |  | | |__  | |__) |
 |  __|   | | | . ` | |  | |  __| |  _  /
 | |     _| |_| |\  | |__| | |____| | \ \
 |_|    |_____|_| \_|_____/|______|_|  \_\

										   */

BlockLocation *BlockServer::LocationPositionChecker(const std::string &part) {
	std::vector<BlockLocation>::iterator it;

	for (it = _locations.begin(); it != _locations.end(); ++it) {
		if (part.find(it->getPath()) == 0) return &(*it);
	}
	return NULL;
}

BlockLocation *BlockServer::getLocationByPath(const std::string &path) {
	std::vector<BlockLocation>::iterator it;

	for (it = _locations.begin(); it != _locations.end(); ++it) {
		if (strcmp(it->getPath().c_str(), path.c_str()) == 0) return &(*it);
	}
	return NULL;
}

/*_____        _____   _____ _____ _   _  _____
 |  __ \ /\   |  __ \ / ____|_   _| \ | |/ ____|
 | |__) /  \  | |__) | (___   | | |  \| | |  __
 |  ___/ /\ \ |  _  / \___ \  | | | . ` | | |_ |
 | |  / ____ \| | \ \ ____) |_| |_| |\  | |__| |
 |_| /_/    \_\_|  \_\_____/|_____|_| \_|\_____|

												 */

bool BlockServer::VerifEmptyRRI() {
	std::vector<BlockLocation>::iterator it;

	for (it = _locations.begin(); it != _locations.end(); ++it) {
		std::map<std::string, int> counterbase = it->getCounterBase();
		Log::log(Log::DEBUG, "Location : %s", it->getPath().c_str());
		Log::log(Log::DEBUG, "Root : %d", counterbase["root"]);
		Log::log(Log::DEBUG, "Rewrite : %d", counterbase["rewirte"]);
		Log::log(Log::DEBUG, "AutoIndex : %d", counterbase["autoindex"]);
		if (!(counterbase["root"]) && !(counterbase["autoindex"]) &&
			!(counterbase["rewrite"])) {
			Log::log(Log::FATAL, "Invalid Config %s", it->getPath().c_str());
			return false;
		}
	}
	return true;
}

BlockServer BlockServer::getServerConfig(std::ifstream &configFile) {
	std::string line;
	std::vector<std::string> tokens;
	std::string key;
	bool isCloseServer = false;

	while (std::getline(configFile, line)) {
		ConfParser::countLineFile++;
		line = trim(line, " \t\n\r\f\v");
		if (line.empty() || line[0] == '#') continue;
		tokens = ft_split(line, ' ');
		key = tokens[0];
		if (key[0] == '}' && key.size() == 1 && tokens.size() == 1) {
			isCloseServer = true;
			break;
		} else if (ValidServerChecker(tokens, key, configFile))
			continue;
		else
			Log::log(Log::FATAL, "Invalid line: \"%s\" in file: %s:%d",
					 line.c_str(), _filename.c_str(),
					 ConfParser::countLineFile);
	}
	if (isCloseServer == false && !EmptyFileChecker())
		Log::log(Log::FATAL, "Missing } in file %s:%d", _filename.c_str(),
				 ConfParser::countLineFile);
	if ((!DoubleLocationChecker()) || (!DoubleLineChecker()) ||
		(!VerifEmptyRRI()))
		Log::log(Log::FATAL, "Invalid Config %s:%d", _filename.c_str(),
				 ConfParser::countLineFile);
	cleanPaths();
	return (*this);
}
