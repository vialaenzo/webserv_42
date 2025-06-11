#ifndef USEFULL_HPP
#define USEFULL_HPP

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <string>
#include <vector>

#define BUFFER_SIZE 4096
#define MAX_EVENTS 100
#define PORT 1234
#define REQUEST_FLAGS EPOLLIN | EPOLLRDHUP | EPOLLERR
#define RESPONSE_FLAGS EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT

class Response;
class Server;
class BlockServer;

extern Server* g_server;
extern int fdSocket;

enum e_Methods { GET, POST, DELETE, UNKNOWN };

std::string ullToStr(unsigned long long ull);
std::string ft_itos(int value);
int ft_stoi(std::string str);
std::string ltrim(std::string str, std::string set);
std::string rtrim(std::string str, std::string set);
std::string trim(std::string str, std::string set);
bool EmptyFileChecker();
std::vector<std::string> ft_split(const std::string& str, char delimiter);
std::string UIntToString(unsigned int value);
int extractPort(const std::string& server);
std::string extractIp(const std::string& server);
std::string e_Methods_to_String(e_Methods method);
e_Methods str_to_e_Methods(std::string str);
int VerifFatalCallFonc(int ret, std::string msg, bool isFatal = true);
void handle_signal(int signal);
Response createResponseError(std::string, std::string, std::string);
Response createResponseError(BlockServer*, std::string, std::string,
							 std::string);
std::string getFileContent(std::string path);
std::string getMimeType(std::string target);
std::string parseFileExtension(const std::string&);
std::string unescapeJsonString(const std::string& escaped);
std::string urlDecode(const std::string& value);

#endif
