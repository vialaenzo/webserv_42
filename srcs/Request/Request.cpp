#include "Request.hpp"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "usefull.hpp"

Request::Request()
	: _method(UNKNOWN),
	  _target(""),
	  _protocol(""),
	  _headers(),
	  _body(""),
	  _rawBody("") {};

Request::Request(const std::string &raw)
	: _method(parseMethod(raw)),
	  _target(parseTarget(raw)),
	  _protocol(parseProtocol(raw)),
	  _headers(parseHeaders(raw)),
	  _body(parseBody(raw)),
	  _rawBody("") {
	this->_rawBody = this->_body;
	if (this->_headers.find("Transfer-Encoding") != this->_headers.end() &&
		_headers["Transfer-Encoding"] == "chunked")
		this->_body = Request::unchunkBody(this->_rawBody);

	std::string content_length = this->_headers["Content-Length"];
	if (!content_length.empty()) {
		size_t length = ft_stoi(content_length);
		if (length != this->_body.length())
			throw(std::runtime_error(
				"Mismatch between the 'Content-Length' and the actual length"));
	}
};
Request::Request(const Request &other)
	: _method(other._method),
	  _target(other._target),
	  _protocol(other._protocol),
	  _headers(other._headers),
	  _body(other._body),
	  _rawBody(other._rawBody) {};
Request &Request::operator=(const Request &other) {
	if (this != &other) {
		this->_method = other._method;
		this->_target = other._target;
		this->_protocol = other._protocol;
		this->_headers = other._headers;
		this->_body = other._body;
		this->_rawBody = other._rawBody;
	}
	return (*this);
};
Request::~Request() {};

e_Methods Request::parseMethod(const std::string &content) {
	std::istringstream str_stream(content);
	std::string line;
	if (!std::getline(str_stream, line))
		throw std::runtime_error("Error: parsing method of request");
	if (line[line.length() - 1] == '\r')
		line = line.substr(0, line.length() - 1);
	line = trim(line, " ");
	size_t end = line.find_first_of(" ");
	if (end == std::string::npos)
		throw std::runtime_error("Error: parsing method of request");
	std::string method = line.substr(0, end);
	return (str_to_e_Methods(method));
};
std::string Request::parseTarget(const std::string &content) {
	std::istringstream str_stream(content);
	std::string line;
	if (!std::getline(str_stream, line))
		throw std::runtime_error("Error: parsing target or request");
	if (line[line.length() - 1] == '\r')
		line = line.substr(0, line.length() - 1);
	line = trim(line, " ");
	size_t start = line.find_first_of(" ") + 1;
	size_t end = line.find_last_of(" ") - 1;
	if (start == std::string::npos || end == std::string::npos)
		throw std::runtime_error("Error: parsing target or request");
	return (line.substr(start, end - start + 1));
};
std::string Request::parseProtocol(const std::string &content) {
	std::istringstream str_stream(content);
	std::string line;
	if (!std::getline(str_stream, line))
		throw std::runtime_error("Error: parsing protocol or request");
	if (line[line.length() - 1] == '\r')
		line = line.substr(0, line.length() - 1);
	line = trim(line, " ");
	size_t start = line.find_last_of(" ") + 1;
	if (start == std::string::npos)
		throw std::runtime_error("Error: parsing protocol or request");
	return (line.substr(start));
};
MapHeaders Request::parseHeaders(const std::string &content) {
	MapHeaders headers;
	size_t sep;
	std::istringstream str_stream(content);
	std::string key, value, line;

	std::getline(str_stream, line);	 // skip the startline
	while (std::getline(str_stream, line)) {
		if (line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() - 1);
		if (line.empty()) break;
		line = trim(line, " ");
		if ((sep = line.find(":")) == std::string::npos)
			throw std::runtime_error(
				"Error: parsing missing ':' in headers of request");
		key = trim(line.substr(0, sep), " ");
		value = trim(line.substr(sep + 1), " ");
		headers[key] = value;
	}
	return (headers);
};
std::string Request::parseBody(const std::string &content) {
	std::istringstream str_stream(content.c_str());
	std::string line;
	size_t delimiter = content.find("\r\n\r\n");
	if (delimiter == std::string::npos)
		throw std::runtime_error("Error: parsing body of request");
	return (content.substr(delimiter + 4));
};

void Request::setMethod(e_Methods method) { this->_method = method; };
void Request::setTarget(std::string target) { this->_target = target; };
void Request::setProtocol(std::string protocol) { this->_protocol = protocol; };
void Request::setHeaders(MapHeaders headers) { this->_headers = headers; };
void Request::setBody(std::string body) { this->_body = body; };

const e_Methods &Request::getMethod() const { return (this->_method); };
const std::string &Request::getTarget() const { return (this->_target); };
const std::string &Request::getProtocol() const { return (this->_protocol); };
const MapHeaders &Request::getHeaders() const { return (this->_headers); };
const std::string &Request::getBody() const { return (this->_body); };

std::string Request::parsePath() const {
	std::string::size_type sep = this->_target.find("?");
	if (sep == std::string::npos) return urlDecode(this->_target);
	return urlDecode(this->_target.substr(sep));
};
std::string Request::parseQuery() const {
	std::string::size_type sep = this->_target.find("?");
	if (sep == std::string::npos) return "";
	return (this->_target.substr(sep + 1));
};
std::map<std::string, std::string> Request::parseQueryToMap() {
	std::map<std::string, std::string> map;
	std::string query, line, key, value;
	std::string::size_type pos;

	if ((query = this->parseQuery()).empty()) return map;
	std::stringstream ss(query);
	while (std::getline(ss, line, '&')) {
		pos = line.find('=');
		if (pos == std::string::npos) continue;	 // might want to throw an error
		key = urlDecode(line.substr(0, pos));
		value = urlDecode(line.substr(pos + 1));
		map[key] = value;
	}
	return map;
};

std::string Request::toString() const {
	std::string str;
	std::string method;
	method = e_Methods_to_String(this->getMethod());
	str += method + " " + this->getTarget() + " " + this->getProtocol();
	str += "\r\n";
	MapHeaders headers = this->getHeaders();
	for (MapHeaders::iterator it = headers.begin(); it != headers.end(); it++)
		str = str + it->first + ": " + it->second + "\r\n";
	str += "\r\n";
	str = str + this->getBody();
	return (str);
};

std::string Request::unchunkBody(std::string chunked) {
	size_t chunkSize, last;
	std::istringstream ss(chunked);
	std::ostringstream result;
	std::string unchunked, line;

	while (std::getline(ss, line)) {
		last = line.size() - 1;
		if (!line.empty() && line[last] == '\r') line.erase(last);

		std::istringstream hexStream(line);
		hexStream >> std::hex >> chunkSize;

		if (hexStream.fail())
			throw std::runtime_error("Error: invalid chunk size format");
		if (chunkSize == 0) break;

		char *buffer = new char[chunkSize];
		ss.read(buffer, chunkSize);
		if (ss.fail())
			throw std::runtime_error("Error: [read] unchunking failed.");
		result.write(buffer, chunkSize);
		if (result.fail())
			throw std::runtime_error("Error: [write] unchunking failed.");
		delete[] buffer;

		ss.ignore(2);
	}
	return (result.str());
};

bool Request::isComplete(std::string raw) {
	size_t headerEnd = raw.find("\r\n\r\n");
	if (headerEnd == std::string::npos) return false;

	if (raw.find("Transfer-Encoding: chunked") != std::string::npos &&
		raw.find("\r\n0\r\n\r\n", headerEnd) == std::string::npos)
		return false;

	size_t contentLengthPos = raw.find("Content-Length:");
	if (contentLengthPos != std::string::npos) {
		size_t start = contentLengthPos + 15;
		size_t end = raw.find("\r\n", start);
		if (end == std::string::npos) return false;

		std::string contentLengthStr = raw.substr(start, end - start);
		size_t contentLength = ft_stoi(contentLengthStr);

		size_t bodyStart = headerEnd + 4;
		size_t actualBodyLength = raw.size() - bodyStart;

		if (actualBodyLength < contentLength) return false;
	}

	return true;
};
