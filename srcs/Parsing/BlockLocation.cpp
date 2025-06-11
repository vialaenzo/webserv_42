#include "BlockLocation.hpp"

#include <stdlib.h>

#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

#include "ConfParser.hpp"
#include "Log.hpp"

BlockLocation::BlockLocation(std::string filename)
	: _autoindex(FALSE), _filename(filename) {
	setDefaultValues();
}

BlockLocation::BlockLocation(const BlockLocation &copy) { *this = copy; }

BlockLocation::~BlockLocation() {}

BlockLocation &BlockLocation::operator=(const BlockLocation &copy) {
	if (this != &copy) {
		_path = copy._path;
		_root = copy._root;
		_rewrite = copy._rewrite;
		_alias = copy._alias;
		_indexes = copy._indexes;
		_allowedMethods = copy._allowedMethods;
		_autoindex = copy._autoindex;
		_cgiExtension = copy._cgiExtension;
		_uploadPath = copy._uploadPath;
		_counterBase = copy._counterBase;
		_filename = copy._filename;
	}
	return (*this);
}

/*_____      _            _
 |  __ \    (_)          | |
 | |__) | __ ___   ____ _| |_ ___
 |  ___/ '__| \ \ / / _` | __/ _ \
 | |   | |  | |\ V / (_| | ||  __/
 |_|   |_|  |_| \_/ \__,_|\__\___|

								   */

void BlockLocation::addValidMethod(std::vector<std::string> &tokens) {
	e_Methods met;

	incrementCounter("allowedMethods");
	for (size_t i = 1; i < tokens.size(); i++) {
		std::string token = tokens[i];
		if (ConfParser::CheckerMethod(token) == false) {
			Log::log(Log::FATAL, "Invalid method: \"%s\" in file: %s:%d",
					 token.c_str(), _filename.c_str(),
					 ConfParser::countLineFile);
			exit(Log::FATAL);
		} else if (token == "POST")
			met = POST;
		else if (token == "DELETE")
			met = DELETE;
		else
			met = GET;
		if (std::find(_allowedMethods.begin(), _allowedMethods.end(), met) !=
			_allowedMethods.end()) {
			Log::log(Log::FATAL, "Duplicate method: \"%s\" in file: %s:%d",
					 token.c_str(), _filename.c_str(),
					 ConfParser::countLineFile);
			exit(Log::FATAL);
		}
		_allowedMethods.push_back(met);
	}
}
void BlockLocation::addIndexes(std::vector<std::string> &token) {
	for (size_t i = 1; i < token.size(); ++i) {
		if (_indexSet.find(token[i]) == _indexSet.end()) {
			_indexes.push_back(token[i]);
			_indexSet.insert(token[i]);
		}
	}
}

void BlockLocation::addCgiExtension(std::vector<std::string> &token) {
	if (token.size() != 3) {
		Log::log(Log::FATAL,
				 "Invalid cgi extension in file: %s:%d. Expected 3 elements, "
				 "got %zu",
				 _filename.c_str(), ConfParser::countLineFile, token.size());
		exit(Log::FATAL);
	}

	if (token[1][0] != '.') {
		Log::log(
			Log::FATAL,
			"Invalid cgi extension: \"%s\" in file: %s:%d. Must start with '.'",
			token[1].c_str(), _filename.c_str(), ConfParser::countLineFile);
		exit(Log::FATAL);
	}

	if (_cgiExtension.find(token[1]) != _cgiExtension.end()) {
		Log::log(Log::FATAL, "Duplicate cgi extension: \"%s\" in file: %s:%d",
				 token[1].c_str(), _filename.c_str(),
				 ConfParser::countLineFile);
		exit(Log::FATAL);
	}

	_cgiExtension[token[1]] = token[2];
}

/* Methods */

e_boolMod BlockLocation::strToBool(std::string &str) {
	if (str == "on")
		return (TRUE);
	else if (str == "off")
		return (FALSE);
	else {
		Log::log(Log::FATAL, "Invalid autoindex value: \"%s\" in file: %s:%d",
				 str.c_str(), _filename.c_str(), ConfParser::countLineFile);
		exit(Log::FATAL);
	}
}

void BlockLocation::setDefaultValues() {
	_counterBase["root"] = 0;
	_counterBase["alias"] = 0;
	_counterBase["allowedMethods"] = 0;
	_counterBase["autoindex"] = 0;
	_counterBase["upload_path"] = 0;
	_counterBase["rewrite"] = 0;
}

// bool BlockLocation::DuplicateLineChecker()
//{
//	std::map<std::string, int>::iterator it;

//	for (it = _counterBase.begin(); it != _counterBase.end(); ++it)
//		if (it->second > 1)
//		{
//			Log::log(Log::FATAL, "Duplicate line in location context: %s",
//					 it->first.c_str());
//			return false;
//		}
//	if (_counterBase["root"] > 0 && _counterBase["alias"] > 0)
//	{
//		Log::log(Log::FATAL,"Alias and Root can't be set in same location bloc
//%s", _path.c_str()); 		return false;
//	}
//	return true;
//}

bool BlockLocation::DuplicateLineChecker() {
	std::map<std::string, int>::iterator it;
	std::string msg;

	for (it = _counterBase.begin(); it != _counterBase.end(); ++it) {
		if (it->second > 1) {
			msg = "Duplicate line in location context: %s";
			Log::log(Log::FATAL, msg.c_str(), it->first.c_str());
			return false;
		}
	}
	// checker si root est present, alias ne doit pas l'etre et inversement
	if (_counterBase["root"] > 0 && _counterBase["alias"] > 0) {
		msg = "Alias and Root can't be set in same location bloc %s";
		Log::log(Log::FATAL, msg.c_str(), _path.c_str());
		return false;
	}
	return true;
}

bool BlockLocation::ValidLocationChecker(std::vector<std::string> &tokens,
										 std::string &key) {
	if (tokens.size() < 2) return false;
	if (key == "root" && tokens.size() == 2)
		setRoot(tokens[1]);
	else if (key == "autoindex")
		setAutoIndex(strToBool(tokens[1]));
	else if (key == "return" && tokens.size() == 3)
		setRewrite(tokens);
	else if (key == "alias" && tokens.size() == 2)
		setAlias(tokens[1]);
	else if (key == "allow_methods")
		addValidMethod(tokens);
	else if (key == "index")
		addIndexes(tokens);
	else if (key == "cgi_extension" && tokens.size() == 3)
		addCgiExtension(tokens);
	else if (key == "upload_path" && tokens.size() == 2)
		setUploadPath(tokens[1]);
	else
		return false;
	return true;
}

/*_____  _    _ ____  _      _____ _____
 |  __ \| |  | |  _ \| |    |_   _/ ____|
 | |__) | |  | | |_) | |      | || |
 |  ___/| |  | |  _ <| |      | || |
 | |    | |__| | |_) | |____ _| || |____
 |_|     \____/|____/|______|_____\_____|

										  */

void BlockLocation::setRewrite(std::vector<std::string> &tokens) {
	int code = std::atoi(tokens[1].c_str());
	if (code < 300 || code > 399) {
		Log::log(Log::FATAL, "Invalid return code: \"%s\" in file: %s:%d",
				 tokens[1].c_str(), _filename.c_str(),
				 ConfParser::countLineFile);
		exit(Log::FATAL);
	}
	_counterBase["rewrite"]++;
	_rewrite = std::make_pair(code, tokens[2]);
}

BlockLocation BlockLocation::getLocationConfig(std::ifstream &configFile,
											   std::string &path) {
	std::string line;
	std::vector<std::string> tokens;
	std::string key;
	bool isCloseLocation = false;

	setPath(path);
	while (std::getline(configFile, line)) {
		ConfParser::countLineFile++;
		line = trim(line, " \t");
		if (line.empty() || line[0] == '#') continue;
		tokens = ft_split(line, ' ');
		key = tokens[0];
		if (key[0] == '}' && key.size() == 1 && tokens.size() == 1) {
			isCloseLocation = true;
			break;
		}
		if (ValidLocationChecker(tokens, key))
			continue;
		else {
			Log::log(Log::FATAL, "Invalid line: \"%s\" in file: %s:%d",
					 line.c_str(), _filename.c_str(),
					 ConfParser::countLineFile);
			exit(Log::FATAL);
		}
	}
	if (!isCloseLocation && !EmptyFileChecker()) {
		Log::log(Log::FATAL, "Missing } in file: %s:%d", _filename.c_str(),
				 ConfParser::countLineFile);
		exit(Log::FATAL);
	}
	if (!DuplicateLineChecker()) exit(Log::FATAL);
	// if (Log::getLogDebugState())
	//	printLocation();
	return (*this);
}

bool BlockLocation::isMethodAllowed(e_Methods method) {
	return (std::find(_allowedMethods.begin(), _allowedMethods.end(), method) !=
			_allowedMethods.end());
}

void BlockLocation::cleanPaths() {
	if (!_root.empty() && _root != "/" && _root[_root.size() - 1] == '/')
		_root.erase(_root.size() - 1);

	if (!_alias.empty() && _alias != "/" && _alias[_alias.size() - 1] == '/')
		_alias.erase(_alias.size() - 1);

	if (!_uploadPath.empty() && _uploadPath != "/" &&
		_uploadPath[_uploadPath.size() - 1] == '/')
		_uploadPath.erase(_uploadPath.size() - 1);
}

// ------------------------------- UTILS --------------------------------
e_Methods BlockLocation::ConvertStrtoMethod(const std::string &method) {
	if (method == "GET") return (GET);
	if (method == "POST") return (POST);
	if (method == "DELETE") return (DELETE);
	return (UNKNOWN);
}

/*_____      _       _
 |  __ \    (_)     | |
 | |__) | __ _ _ __ | |_ ___ _ __ ___
 |  ___/ '__| | '_ \| __/ _ \ '__/ __|
 | |   | |  | | | | | ||  __/ |  \__ \
 |_|   |_|  |_|_| |_|\__\___|_|  |___/
									   */

void BlockLocation::printPair(const std::string &title,
							  const std::string &value) {
	std::cout << std::setw(15) << std::left << title << ": "
			  << (value.empty() ? "none" : value) << std::endl;
}

void BlockLocation::printBool(const std::string &title, bool value,
							  const std::string &trueStr,
							  const std::string &falseStr) {
	std::cout << std::setw(15) << std::left << title << ": "
			  << (value ? trueStr : falseStr) << std::endl;
}

void BlockLocation::printVector(const std::string &title,
								const std::vector<std::string> &vec) {
	std::cout << std::setw(15) << std::left << title << ": "
			  << (vec.empty() ? "none" : "") << std::endl;
	for (std::vector<std::string>::const_iterator it = vec.begin();
		 it != vec.end(); ++it)
		std::cout << "\t- " << *it << std::endl;
}

void BlockLocation::printMap(const std::string &title,
							 const std::map<std::string, std::string> &map) {
	std::cout << std::setw(15) << std::left << title << ": "
			  << (map.empty() ? "none" : "") << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = map.begin();
		 it != map.end(); ++it)
		std::cout << "\t- " << it->first << ": " << it->second << std::endl;
}

void BlockLocation::printLocation(void) {
	printPair("Path", _path);
	printPair("Root", _root);
	printPair("Alias", _alias);
	printMap("CGI extension", _cgiExtension);
	printPair("Upload path", _uploadPath);
	printBool("Autoindex", _autoindex == TRUE, "on", "off");

	std::cout << std::setw(15) << std::left << "Rewrite" << ": "
			  << (_rewrite.first != 0
					  ? ft_itos(_rewrite.first) + " " + _rewrite.second
					  : "none")
			  << std::endl;

	printVector("Indexes", _indexes);

	std::cout << std::setw(15) << std::left << "Allowed methods" << ": "
			  << std::endl;
	for (std::vector<e_Methods>::const_iterator it = _allowedMethods.begin();
		 it != _allowedMethods.end(); ++it) {
		std::cout << "\t- ";
		if (*it == GET)
			std::cout << "GET";
		else if (*it == POST)
			std::cout << "POST";
		else if (*it == DELETE)
			std::cout << "DELETE";
		else
			std::cout << "UNKNOWN";
		std::cout << std::endl;
	}
}
