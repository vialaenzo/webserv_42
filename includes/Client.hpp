#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <netinet/in.h>
#include <sys/socket.h>

#include <ctime>

#include "Request.hpp"
#include "Response.hpp"
#include "Socket.hpp"

#define CLIENT_BUFFER_SIZE 8192	 // 4096

class Request;
class Response;

class Client {
private:
	int _fd;
	Socket* _socket;
	Request* _request;
	Response* _response;
	std::string _requestBuffer;
	time_t _lastActivity;

public:
	Client(int fd, Socket* socket);
	~Client(void);

	/* HANDLE */

	void reset(void);

	/* GETTERS */
	int getFd(void) const { return _fd; }
	Request* getRequest(void) const { return _request; }
	Socket* getSocket(void) const { return _socket; }
	Response* getResponse(void) const { return _response; }
	std::string getRequestBuffer(void) const { return _requestBuffer; }

	/* SETTERS */
	void setRequest(Request* request) {
		if (this->_request) delete this->_request;
		this->_request = request;
	}
	void setResponse(Response* response) {
		if (this->_response) delete this->_response;
		this->_response = response;
	}
	void setRequestBuffer(std::string raw) { this->_requestBuffer = raw; };

	void appendRequestBuffer(std::string raw) { this->_requestBuffer += raw; };

	// timeout
	time_t getLastActivity() const { return _lastActivity; }
	void updateActivity() { _lastActivity = time(NULL); }

	// Checkers
	void checkCgi(void);

	class DisconnectedException : public std::exception {
	public:
		virtual const char* what() const throw() {
			return "Client disconnected";
		}
	};
};

#endif
