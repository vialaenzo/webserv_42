#ifndef CGI_HANDLER
#define CGI_HANDLER

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Log.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "usefull.hpp"

class Log;
class Response;
class Request;
class Server;

#define CGI_TIMEOUT 10	// Timeout en secondes

class CgiHandler {
	friend class Response;

private:
	std::string _output;
	std::string _target;
	std::string _cgi_path;
	std::string _cgi_extension;
	bool _isChunked;

public:
	CgiHandler();
	CgiHandler(const CgiHandler &src);
	~CgiHandler(void);

	CgiHandler &operator=(const CgiHandler &src);

	/*Getters*/

	Response CgiMaker(const Request *request, const BlockLocation *location,
					  BlockServer *server);

	std::vector<std::string> createCgiEnv(const Request *request);

	Response executeCgi(const Request *request, BlockServer *server);
};

#endif
