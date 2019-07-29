#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BACKLOG 50
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

const size_t g_buffSize = 64;
const static char* g_portStr = "20001";

void fatalError(const char* msg);

ssize_t readLine(int fd, void *buffer, size_t n);
