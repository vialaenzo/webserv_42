#include <stdint.h>

void addSocketEpoll(int epollFD, int sockFD, uint32_t flags);
void modifySocketEpoll(int epollFD, int sockFD, uint32_t flags);
void deleteSocketEpoll(int epollFD, int sockFD);
