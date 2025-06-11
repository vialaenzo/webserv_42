/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtrala <dtrala@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 23:50:42 by dtrala            #+#    #+#             */
/*   Updated: 2025/03/12 12:57:00 by dtrala           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "usefull.hpp"

Response::Response()
	: _protocol(""), _status_code(""), _status_text(""), _headers(), _body("") {
	  };

Response::Response(std::string raw)
	: _protocol(parseProtocol(raw)),
	  _status_code(parseStatusCode(raw)),
	  _status_text(parseStatusText(raw)),
	  _headers(parseHeaders(raw)),
	  _body(parseBody(raw)) {};
;
Response::Response(const Response &other)
	: _protocol(other._protocol),
	  _status_code(other._status_code),
	  _status_text(other._status_text),
	  _headers(other._headers),
	  _body(other._body) {};
Response &Response::operator=(const Response &other) {
	if (this != &other) {
		this->_protocol = other._protocol;
		this->_status_code = other._status_code;
		this->_status_text = other._status_text;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return (*this);
};
Response::~Response() {};

std::string Response::parseProtocol(const std::string &content) {
	std::istringstream str_stream(content);
	std::string line;
	if (!std::getline(str_stream, line))
		throw std::runtime_error("Error: parsing protocol of response");
	if (line[line.length() - 1] == '\r')
		line = line.substr(0, line.length() - 1);
	line = trim(line, " ");
	size_t end = line.find_first_of(" ");
	if (end == std::string::npos)
		throw std::runtime_error("Error: parsing protocol of response");
	return (line.substr(0, end));
};
std::string Response::parseStatusCode(const std::string &content) {
	std::istringstream str_stream(content);
	std::string line;
	if (!std::getline(str_stream, line))
		throw std::runtime_error("Error: parsing status_code or response");
	if (line[line.length() - 1] == '\r')
		line = line.substr(0, line.length() - 1);
	line = trim(line, " ");
	size_t start = line.find_first_of(" ") + 1;
	size_t end = line.find_last_of(" ") - 1;
	if (start == std::string::npos || end == std::string::npos)
		throw std::runtime_error("Error: parsing status_code or response");
	return (line.substr(start, end - start + 1));
};
std::string Response::parseStatusText(const std::string &content) {
	std::istringstream str_stream(content);
	std::string line;
	if (!std::getline(str_stream, line))
		throw std::runtime_error("Error: parsing status_text or response");
	if (line[line.length() - 1] == '\r')
		line = line.substr(0, line.length() - 1);
	line = trim(line, " ");
	size_t start = line.find_last_of(" ") + 1;
	if (start == std::string::npos)
		throw std::runtime_error("Error: parsing status_text or response");
	return (line.substr(start));
};
std::map<std::string, std::string> Response::parseHeaders(
	const std::string &content) {
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
				"Error: parsing missing ':' in headers of response");
		key = trim(line.substr(0, sep), " ");
		value = trim(line.substr(sep + 1), " ");
		headers[key] = value;
	}
	return (headers);
};
std::string Response::parseBody(const std::string &content) {
	std::istringstream str_stream(content.c_str());
	std::string line;
	size_t delimiter = content.find("\r\n\r\n");
	if (delimiter == std::string::npos)
		throw std::runtime_error("Error: parsing body of request");
	return (content.substr(delimiter + 4));
};

// I should add some checks
void Response::setProtocol(std::string protocol) {
	this->_protocol = protocol;
};
void Response::setStatusCode(std::string code) { this->_status_code = code; };
void Response::setStatusText(std::string text) { this->_status_text = text; };
void Response::setHeaders(MapHeaders headers) { this->_headers = headers; };
void Response::setBody(std::string body) { this->_body = body; };

const std::string &Response::getProtocol() const { return (this->_protocol); };
const std::string &Response::getStatusCode() const {
	return (this->_status_code);
};
const std::string &Response::getStatusText() const {
	return (this->_status_text);
};
const MapHeaders &Response::getHeaders() const { return (this->_headers); };
const std::string &Response::getBody() const { return (this->_body); };

std::string Response::toString() const {
	std::string str;
	str += this->getProtocol() + " " + this->getStatusCode() + " " +
		   this->getStatusText() + "\r\n";
	MapHeaders headers = this->getHeaders();
	for (MapHeaders::iterator it = headers.begin(); it != headers.end(); it++)
		str += it->first + ": " + it->second + "\r\n";
	str += "\r\n";
	str += this->getBody();
	return (str);
};
