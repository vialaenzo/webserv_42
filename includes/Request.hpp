/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtrala <dtrala@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 23:49:03 by dtrala            #+#    #+#             */
/*   Updated: 2025/03/26 13:13:49 by dtrala           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <ostream>
#include <string>

#include "Socket_utils.hpp"
#include "usefull.hpp"

typedef std::map<std::string, std::string> MapHeaders;
typedef std::map<std::string, std::string> MapJson;

#define REQUEST_DEFAULT_STEP_CODE 200
#define REQUEST_DEFAULT_UPLOAD_PATH "./web/upload/"
#define REQUEST_DEFAULT_HEADER_TIMEOUT 10
#define REQUEST_DEFAULT_BODY_TIMEOUT 3600
#define REQUEST_DEFAULT_CGI_TIMEOUT 3

class Client;
class BlockLocation;
class BlockServer;
class Socket;

class Request {
public:
	Request();
	Request(const std::string &content);
	Request(const Request &other);
	Request &operator=(const Request &other);
	~Request();

	e_Methods parseMethod(const std::string &content);
	std::string parseTarget(const std::string &content);
	std::string parseProtocol(const std::string &content);
	std::map<std::string, std::string> parseHeaders(const std::string &content);
	std::string parseBody(const std::string &content);

	void setMethod(e_Methods method);
	void setTarget(std::string target);
	void setProtocol(std::string protocol);
	void setHeaders(std::map<std::string, std::string> headers);
	void setBody(std::string body);

	const e_Methods &getMethod() const;
	const std::string &getTarget() const;
	const std::string &getProtocol() const;
	const std::map<std::string, std::string> &getHeaders() const;
	const std::string &getBody() const;
	const std::string &getRawBody() const;

	std::string parsePath() const;
	std::string parseQuery() const;
	std::map<std::string, std::string> parseQueryToMap();

	std::string toString() const;

	static std::string unchunkBody(std::string chunked);
	static bool isComplete(std::string raw);

protected:
private:
	e_Methods _method;
	std::string _target;
	std::string _protocol;
	std::map<std::string, std::string> _headers;
	std::string _body;
	std::string _rawBody;
};

std::ostream &operator<<(std::ostream &os, const Request &request);

#endif
