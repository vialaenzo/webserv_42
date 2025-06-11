/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtrala <dtrala@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 23:51:05 by dtrala            #+#    #+#             */
/*   Updated: 2025/03/20 10:30:59 by dtrala           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

class Request;

typedef std::map<std::string, std::string> MapHeaders;

class Response {
public:
	Response();
	Response(std::string raw);
	Response(const Response &other);
	Response &operator=(const Response &other);
	~Response();

	std::string parseProtocol(const std::string &content);
	std::string parseStatusCode(const std::string &content);
	std::string parseStatusText(const std::string &content);
	std::map<std::string, std::string> parseHeaders(const std::string &content);
	std::string parseBody(const std::string &content);

	void setProtocol(std::string protocol);
	void setStatusCode(std::string code);
	void setStatusText(std::string text);
	void setHeaders(std::map<std::string, std::string> headers);
	void setBody(std::string body);

	const std::string &getProtocol() const;
	const std::string &getStatusCode() const;
	const std::string &getStatusText() const;
	const std::map<std::string, std::string> &getHeaders() const;
	const std::string &getBody() const;

	std::string toString() const;

protected:
private:
	std::string _protocol;
	std::string _status_code;
	std::string _status_text;
	std::map<std::string, std::string> _headers;
	std::string _body;
};

std::ostream &operator<<(std::ostream &os, const Response &response);

#endif
