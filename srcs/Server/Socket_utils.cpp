#include "Socket_utils.hpp"

#include <sys/epoll.h>

#include "usefull.hpp"

void addSocketEpoll(int epollFD, int sockFD, uint32_t flags) {
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	VerifFatalCallFonc(epoll_ctl(epollFD, EPOLL_CTL_ADD, sockFD, &ev),
					   "Error with epoll_ctl function", false);
}

void modifySocketEpoll(int epollFD, int sockFD, uint32_t flags) {
	epoll_event ev;
	ev.events = flags;
	ev.data.fd = sockFD;
	VerifFatalCallFonc(epoll_ctl(epollFD, EPOLL_CTL_MOD, sockFD, &ev),
					   "Error with epoll_ctl function", false);
}

void deleteSocketEpoll(int epollFD, int sockFD) {
	epoll_event ev;
	ev.data.fd = sockFD;
	VerifFatalCallFonc(epoll_ctl(epollFD, EPOLL_CTL_DEL, sockFD, &ev),
					   "Error with epoll_ctl function", false);
}
