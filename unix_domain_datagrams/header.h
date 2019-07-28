#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define BACKLOG 5

const size_t g_buffSize = 64;
const char* g_socketName = "/tmp/domain_datagrams_socket";

void fatalError(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}
