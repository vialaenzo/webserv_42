#ifndef BLOCKLOCATION_HPP
#define BLOCKLOCATION_HPP

#include <unistd.h>

#include <cstring>
#include <map>
#include <set>
#include <vector>

#include "usefull.hpp"

enum e_boolMod { FALSE = 0, TRUE };

class BlockLocation {
private:
	std::string _path;
	std::string _root;
	std::pair<int, std::string> _rewrite;
	std::string _alias;
	std::vector<std::string> _indexes;
	std::vector<e_Methods> _allowedMethods;
	e_boolMod _autoindex;
	std::map<std::string, std::string> _cgiExtension;
	std::string _uploadPath;
	std::set<std::string> _indexSet;

	// divers
	std::map<std::string, int> _counterBase;
	std::string _filename;

	bool ValidLocationChecker(std::vector<std::string> &tokens,
							  std::string &key);

	// Methods
	void incrementCounter(const std::string &key) { _counterBase[key]++; }
	bool DuplicateLineChecker();
	void setDefaultValues();
	e_boolMod strToBool(std::string &str);

	// Adders
	void addValidMethod(std::vector<std::string> &tokens);
	void addIndexes(std::vector<std::string> &token);
	void addCgiExtension(std::vector<std::string> &token);

public:
	BlockLocation(std::string filename);
	BlockLocation(const BlockLocation &copy);
	BlockLocation &operator=(const BlockLocation &copy);
	~BlockLocation();

	// parser
	BlockLocation getLocationConfig(std::ifstream &configFile,
									std::string &path);

	// Getters
	const std::string &getPath() const { return _path; }
	const std::string &getRoot() const { return _root; }
	const std::pair<int, std::string> &getRewrite() const { return _rewrite; }
	const std::string &getAlias() const { return _alias; }
	const std::vector<std::string> &getIndexes() const { return _indexes; }
	const std::string &getUploadPath() const { return _uploadPath; }
	const std::map<std::string, std::string> &getCGI() const {
		return _cgiExtension;
	}
	const std::vector<std::string> &getFiles() const { return _indexes; }
	const std::vector<e_Methods> &getAllowedMethods() const {
		return _allowedMethods;
	}
	std::string getCgiPath(const std::string &path) const {
		return _cgiExtension.at(path);
	}
	e_boolMod getAutoIndex() const { return _autoindex; }
	std::map<std::string, int> getCounterBase() const { return _counterBase; }

	// Setters
	void setPath(const std::string &path) { _path = path; }
	void setUploadPath(const std::string &uploadPath) {
		_uploadPath = uploadPath;
		_counterBase["upload_path"]++;
	}
	void setRoot(const std::string &root) {
		_root = root;
		_counterBase["root"]++;
	}
	void setRewrite(std::vector<std::string> &tokens);
	void setAlias(const std::string &alias) {
		_alias = alias;
		_counterBase["alias"]++;
	}
	void setAutoIndex(e_boolMod autoindex) {
		_autoindex = autoindex;
		_counterBase["autoindex"]++;
	}

	// METHODS

	// Checker
	bool isEnableCgi(const std::string &path) const {
		return _cgiExtension.find(path) != _cgiExtension.end();
	}

	// Display
	void printLocation(void);
	void printPair(const std::string &label, const std::string &value);
	void printBool(const std::string &label, bool value,
				   const std::string &trueStr, const std::string &falseStr);
	void printVector(const std::string &label,
					 const std::vector<std::string> &vec);
	void printMap(const std::string &label,
				  const std::map<std::string, std::string> &map);

	// USEFULL
	static e_Methods ConvertStrtoMethod(const std::string &method);
	void cleanPaths();
	bool isMethodAllowed(e_Methods method);
};

#endif
