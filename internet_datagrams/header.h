#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define BACKLOG 5

const size_t g_buffSize = 64;
const int g_portNum = 20000;

void fatalError(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}
