#include "Client.hpp"

#include "Log.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "usefull.hpp"

Client::Client(int fd, Socket* socket)
	: _fd(fd),
	  _socket(socket),
	  _request(NULL),
	  _response(NULL),
	  _requestBuffer(""),
	  _lastActivity(time(NULL)) {
	Log::log(Log::DEBUG, "[Client] Initializing client with fd %d", fd);
}

Client::~Client(void) {
	if (this->_fd != -1)
		VerifFatalCallFonc(close(this->_fd),
						   "[~Client] Faild to close client socket", false);
	if (this->_request != NULL) delete this->_request;
	if (this->_response != NULL) delete this->_response;
}

void Client::reset(void) {
	// Reset request
	delete this->_request;
	this->_request = new Request();
	// Reset response
	delete this->_response;
	this->_response = new Response();
}

void Client::checkCgi(void) {
	/*	if (this->_request == NULL) // No request
			return ;
		if (!this->_request->_cgi._isCGI) // Not a CGI
			return ;
		return (this->_request->_cgi._checkState());*/
	Log::log(Log::DEBUG, "[checkCgi] Checking CGI not made");
}
