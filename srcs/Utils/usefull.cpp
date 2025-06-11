#include "usefull.hpp"

#include <stdint.h>

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ConfParser.hpp"
#include "Log.hpp"
#include "Response.hpp"
#include "Server.hpp"

class Log;
class Server;
class Response;
class ConfParser;

int fdSocket = -1;

std::string ullToStr(unsigned long long ull) {
	std::ostringstream oss;
	oss << ull;
	return (oss.str());
}

std::string ft_itos(int value) {
	std::stringstream ss;
	ss << value;
	return (ss.str());
}

int ft_stoi(std::string str) {
	std::stringstream ss;
	int res = 0;
	ss << str;
	ss >> res;
	return (res);
}

bool EmptyFileChecker() { return (ConfParser::countLineFile == 0); }

std::string ltrim(std::string str, std::string set) {
	if (set.empty()) return (str);
	size_t start = str.find_first_not_of(set);
	if (start == std::string::npos) return (str);
	return (str.substr(start));
}
std::string rtrim(std::string str, std::string set) {
	if (set.empty()) return (str);
	size_t end = str.find_last_not_of(set);
	if (end == std::string::npos) return (str);
	return (str.substr(0, end + 1));
}
std::string trim(std::string str, std::string set) {
	if (set.empty()) return (str);
	size_t start = str.find_first_not_of(set);
	size_t end = str.find_last_not_of(set);
	if (start == std::string::npos || end == std::string::npos) return (str);
	return (str.substr(start, end - start + 1));
};

std::vector<std::string> ft_split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);	// Utilise un flux pour découper la chaîne
	std::string token;
	while (std::getline(ss, token, delimiter)) tokens.push_back(token);
	return tokens;
}

std::string UIntToString(unsigned int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

int extractPort(const std::string& server) {
	size_t colonPos = server.find(':');

	if (colonPos != std::string::npos) {
		try {
			return std::atoi((server.substr(colonPos + 1)).c_str());
		} catch (std::runtime_error& e) {
			Log::log(Log::ERROR, e.what());
			return -1;
		}
	}
	return -1;
}

std::string extractIp(const std::string& server) {
	size_t colonPos = server.find(':');

	if (colonPos != std::string::npos) return server.substr(0, colonPos);
	return server;
}

std::string e_Methods_to_String(e_Methods method) {
	if (method == GET) return "GET";
	if (method == DELETE) return "DELETE";
	if (method == POST) return "POST";
	return "UNKNOWN";
}

e_Methods str_to_e_Methods(std::string str) {
	if (str == "GET") return (GET);
	if (str == "POST") return (POST);
	if (str == "DELETE") return (DELETE);
	return (UNKNOWN);
}

int VerifFatalCallFonc(int ret, std::string msg, bool isFatal) {
	if (ret < 0) {
		if (isFatal)
			Log::log(Log::FATAL, msg.c_str());
		else
			Log::log(Log::ERROR, msg.c_str());
	}
	return ret;
}

void handle_signal(int signal) {
	if (signal != SIGINT) return;
	std::cout << "\rStopping the server " << std::endl;
	if (fdSocket != -1) close(fdSocket);
	g_server->stop();
}

Response createResponseError(std::string protocol, std::string status_code,
							 std::string status_text) {
	Response response;
	std::map<std::string, std::string> headers;
	std::ostringstream contentStream;

	// Construire le contenu HTML avec le style intégré
	contentStream << "<html lang=\"fr\">";
	contentStream << "<head>";
	contentStream << "<meta charset=\"UTF-8\">";
	contentStream << "<meta name=\"viewport\" content=\"width=device-width, "
					 "initial-scale=1.0\">";
	contentStream << "<title>Erreur " << status_code << "</title>";
	contentStream << "<style>";
	contentStream
		<< "body, html { margin: 0; padding: 0; height: 100%; display: flex; "
		   "justify-content: center; align-items: center; background-color: "
		   "#f0f0f0; font-family: Arial, sans-serif; }";
	contentStream << ".error-container { text-align: center; }";
	contentStream << ".error-code { font-size: 10rem; color: #ff6b6b; "
					 "animation: fadeInOut 2s infinite; }";
	contentStream << ".error-message { font-size: 2rem; color: #333; }";
	contentStream << ".error-details { font-size: 1.2rem; color: #777; }";
	contentStream << "@keyframes fadeInOut { 0%, 100% { opacity: 0.2; } 50% { "
					 "opacity: 1; } }";
	contentStream << "</style>";
	contentStream << "</head>";
	contentStream << "<body>";
	contentStream << "<div class=\"error-container\">";
	contentStream << "<h1 class=\"error-code\">" << status_code << "</h1>";
	contentStream << "<p class=\"error-message\">" << status_text << "</p>";
	contentStream << "</div>";
	contentStream << "</body>";
	contentStream << "</html>";

	std::string content = contentStream.str();

	headers["Content-Type"] = "text/html";
	std::ostringstream lengthStream;
	lengthStream << content.length();
	headers["Content-Length"] = lengthStream.str();

	response.setProtocol(protocol);
	response.setStatusCode(status_code);
	response.setStatusText(status_text);
	response.setHeaders(headers);
	response.setBody(content);

	return response;
};

Response createResponseError(BlockServer* server, std::string protocol,
							 std::string status_code, std::string status_text) {
	Response response;
	int code;
	std::map<int, std::string> error_pages;
	std::map<std::string, std::string> headers;
	std::ostringstream contentStream;

	error_pages = server->getErrorPages();
	std::stringstream(status_code) >> code;

	if (!error_pages[code].empty()) {
		try {
			contentStream << getFileContent(error_pages[code]);
		} catch (std::exception& e) {
			std::cout << "Error opening file" << std::endl;
			contentStream << "<html lang=\"fr\">";
			contentStream << "<head>";
			contentStream << "<meta charset=\"UTF-8\">";
			contentStream
				<< "<meta name=\"viewport\" content=\"width=device-width, "
				   "initial-scale=1.0\">";
			contentStream << "<title>Erreur " << status_code << "</title>";
			contentStream << "<style>";
			contentStream << "body, html { margin: 0; padding: 0; height: "
							 "100%; display: flex; justify-content: center; "
							 "align-items: center; background-color: #f0f0f0; "
							 "font-family: Arial, sans-serif; }";
			contentStream << ".error-container { text-align: center; }";
			contentStream << ".error-code { font-size: 10rem; color: #ff6b6b; "
							 "animation: fadeInOut 2s infinite; }";
			contentStream << ".error-message { font-size: 2rem; color: #333; }";
			contentStream
				<< ".error-details { font-size: 1.2rem; color: #777; }";
			contentStream << "@keyframes fadeInOut { 0%, 100% { opacity: 0.2; "
							 "} 50% { opacity: 1; } }";
			contentStream << "</style>";
			contentStream << "</head>";
			contentStream << "<body>";
			contentStream << "<div class=\"error-container\">";
			contentStream << "<h1 class=\"error-code\">" << status_code
						  << "</h1>";
			contentStream << "<p class=\"error-message\">" << status_text
						  << "</p>";
			contentStream << "</div>";
			contentStream << "</body>";
			contentStream << "</html>";
		}
	} else {
		contentStream << "<html lang=\"fr\">";
		contentStream << "<head>";
		contentStream << "<meta charset=\"UTF-8\">";
		contentStream << "<meta name=\"viewport\" "
						 "content=\"width=device-width, initial-scale=1.0\">";
		contentStream << "<title>Erreur " << status_code << "</title>";
		contentStream << "<style>";
		contentStream
			<< "body, html { margin: 0; padding: 0; height: 100%; display: "
			   "flex; justify-content: center; align-items: center; "
			   "background-color: #f0f0f0; font-family: Arial, sans-serif; }";
		contentStream << ".error-container { text-align: center; }";
		contentStream << ".error-code { font-size: 10rem; color: #ff6b6b; "
						 "animation: fadeInOut 2s infinite; }";
		contentStream << ".error-message { font-size: 2rem; color: #333; }";
		contentStream << ".error-details { font-size: 1.2rem; color: #777; }";
		contentStream << "@keyframes fadeInOut { 0%, 100% { opacity: 0.2; } "
						 "50% { opacity: 1; } }";
		contentStream << "</style>";
		contentStream << "</head>";
		contentStream << "<body>";
		contentStream << "<div class=\"error-container\">";
		contentStream << "<h1 class=\"error-code\">" << status_code << "</h1>";
		contentStream << "<p class=\"error-message\">" << status_text << "</p>";
		contentStream << "</div>";
		contentStream << "</body>";
		contentStream << "</html>";
	}

	std::string content = contentStream.str();

	headers["Content-Type"] = "text/html";
	headers["Content-Length"] = ft_itos(content.length());

	response.setProtocol(protocol);
	response.setStatusCode(status_code);
	response.setStatusText(status_text);
	response.setHeaders(headers);
	response.setBody(content);

	return response;
};

std::string getFileContent(std::string path) {
	std::ifstream inFile(path.c_str());
	if (!inFile) throw std::runtime_error("Error: opening '" + path + "'");
	std::string content;
	std::string line;
	while (std::getline(inFile, line)) content += line + "\n";
	return (content);
};

std::string getMimeType(std::string target) {
	static std::map<std::string, std::string> mapMime;
	if (mapMime.empty()) {
		mapMime["js"] = "application/javascript";
		mapMime["json"] = "application/json";
		mapMime["gif"] = "image/gif";
		mapMime["jpeg"] = "image/jpeg";
		mapMime["jpg"] = "image/jpeg";
		mapMime["png"] = "image/png";
		mapMime["css"] = "text/css";
		mapMime["html"] = "text/html";
		// might not be the appropriate type
		mapMime["pl"] = "text/plain";
		mapMime["py"] = "text/plain";
		mapMime["sh"] = "text/plain";
		//...
	}

	std::string::size_type start = target.find_last_of(".");
	if (start == std::string::npos) return "application/octet-stream";
	start++;

	std::string extension = target.substr(start);
	std::map<std::string, std::string>::iterator it = mapMime.find(extension);
	if (it != mapMime.end()) return it->second;
	return "application/octet-stream";
}

std::string parseFileExtension(const std::string& path) {
	size_t pos;
	std::string ext;

	if ((pos = path.rfind('.')) == std::string::npos) return ("");
	ext = path.substr(pos);
	return ext;
};

std::string unescapeJsonString(const std::string& escaped) {
	std::string unescaped;
	unescaped.reserve(escaped.size());	// Optimisation mémoire

	for (size_t i = 0; i < escaped.size(); ++i) {
		if (escaped[i] == '\\' && i + 1 < escaped.size()) {
			char nextChar = escaped[i + 1];
			switch (nextChar) {
			case 'n':
				unescaped += '\n';
				break;
			case 't':
				unescaped += '\t';
				break;
			case 'r':
				unescaped += '\r';
				break;
			case 'b':
				unescaped += '\b';
				break;
			case 'f':
				unescaped += '\f';
				break;
			case '\\':
				unescaped += '\\';
				break;
			case '"':
				unescaped += '"';
				break;
			case '/':
				unescaped += '/';
				break;
			case 'u':  // Unicode non supporté en C++98, on le laisse tel quel
				unescaped += "\\u";
				if (i + 5 < escaped.size()) {
					unescaped += escaped.substr(i + 2, 4);
					i += 4;	 // On saute les 4 caractères hexadécimaux
				}
				break;
			default:
				// Séquence inconnue, on la garde telle quelle
				unescaped += '\\';
				unescaped += nextChar;
				break;
			}
			++i;  // On saute le caractère d'échappement traité
		} else {
			unescaped += escaped[i];
		}
	}

	return unescaped;
}

std::string urlDecode(const std::string& value) {
	char ch;
	char* endptr;
	std::string decoded;
	std::string hex;

	for (size_t i = 0; i < value.length(); i++) {
		if (value[i] == '%' && i + 2 < value.length()) {
			hex = value.substr(i + 1, 2);
			ch = (char)strtol(hex.c_str(), &endptr, 16);  // Convert hex to char
			if (*endptr == '\0') {
				decoded += ch;
				i += 2;
			} else {
				decoded += '%';
			}
		} else if (value[i] == '+') {
			decoded += ' ';
		} else {
			decoded += value[i];
		}
	}
	return decoded;
}
