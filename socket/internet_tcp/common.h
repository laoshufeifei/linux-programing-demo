#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

#define BACKLOG 50
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

void fatalError(const char* msg);
ssize_t readLine(int fd, void *buffer, size_t n);

#endif