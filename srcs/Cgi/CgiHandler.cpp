#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::CgiHandler(const CgiHandler &src) { *this = src; }

CgiHandler::~CgiHandler(void) {}

CgiHandler &CgiHandler::operator=(const CgiHandler &src) {
	if (this != &src) {
		this->_output = src._output;
		this->_isChunked = src._isChunked;
	}
	return *this;
}

Response CgiHandler::CgiMaker(const Request *request,
							  const BlockLocation *location,
							  BlockServer *server) {
	this->_target = request->parsePath();
	this->_cgi_extension = parseFileExtension(_target);
	this->_cgi_path = location->getCgiPath(_cgi_extension);
	this->_target = location->getRoot() + this->_target;
	Log::log(Log::DEBUG, "Target : %s | Extension: %s | Path Extension : %s",
			 _target.c_str(), _cgi_extension.c_str(), _cgi_path.c_str());

	return executeCgi(request, server);
}

std::vector<std::string> CgiHandler::createCgiEnv(const Request *request) {
	std::vector<std::string> env_list;

	env_list.push_back("SCRIPT_FILENAME=" + _target);
	env_list.push_back("REQUEST_METHOD=" +
					   e_Methods_to_String(request->getMethod()));
	env_list.push_back("QUERY_STRING=" + request->parseQuery());
	env_list.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env_list.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env_list.push_back("REDIRECT_STATUS=200");

	MapHeaders headers = request->getHeaders();
	if (headers.find("Content-Type") != headers.end())
		env_list.push_back("CONTENT_TYPE=" + headers["Content-Type"]);
	if (headers.find("Content-Length") != headers.end())
		env_list.push_back("CONTENT_LENGTH=" + headers["Content-Length"]);

	if (request->getMethod() == POST && !request->getBody().empty()) {
		std::ostringstream oss;
		oss << request->getBody().size();
		env_list.push_back("CONTENT_LENGTH=" + oss.str());
	}
	return env_list;
}

Response CgiHandler::executeCgi(const Request *request, BlockServer *server) {
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1) {
		return createResponseError(server, "HTTP/1.1", "500",
								   "Internal Server Error");
	}

	pid_t pid = fork();
	if (pid == -1) {
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return createResponseError(server, "HTTP/1.1", "500",
								   "Internal Server Error");
	}

	if (pid == 0) {	 // Processus enfant
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);

		std::vector<std::string> envp_list = createCgiEnv(request);

		std::vector<char *> envp;

		for (size_t i = 0; i < envp_list.size(); i++)
			envp.push_back(const_cast<char *>(envp_list[i].c_str()));

		envp.push_back(NULL);  // Terminez avec un pointeur nul

		std::string ch_path = _target.substr(0, _target.rfind('/'));
		_target = _target.substr(_target.rfind('/') + 1);
		if (chdir(ch_path.c_str()) == -1) exit(1);

		char *args[] = {const_cast<char *>(_cgi_path.c_str()),
						const_cast<char *>(_target.c_str()), NULL};

		if (request->getMethod() == POST && !request->getBody().empty()) {
			int stdin_pipe[2];
			if (pipe(stdin_pipe) == -1) exit(1);
			write(stdin_pipe[1], request->getBody().c_str(),
				  request->getBody().size());
			close(stdin_pipe[1]);
			dup2(stdin_pipe[0], STDIN_FILENO);
			close(stdin_pipe[0]);
		}

		execve(_cgi_path.c_str(), args, &envp[0]);
		exit(1);
	}

	close(pipe_fd[1]);
	int status;
	time_t start_time = time(NULL);
	while (true) {
		pid_t result = waitpid(pid, &status, WNOHANG);
		if (result == pid) break;
		if (result == 0 && time(NULL) - start_time >= CGI_TIMEOUT) {
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			return createResponseError(server, "HTTP/1.1", "504",
									   "Gateway Timeout");
		}
		usleep(100000);
	}

	std::string output;
	char buffer[1024];
	ssize_t bytes_read;
	while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) {
		buffer[bytes_read] = '\0';
		output += buffer;
	}
	close(pipe_fd[0]);
	if (bytes_read < 0) throw Client::DisconnectedException();

	if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
		return createResponseError(server, "HTTP/1.1", "502", "Bad Gateway");
	}

	output = "HTTP/1.1 200 OK\r\n" + output;
	return Response(output);
}
