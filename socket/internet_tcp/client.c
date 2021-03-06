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

#include "common.h"

const size_t g_buffSize = 64;

int main(int argc, char *argv[])
{
    if (argc < 4 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printf("usage: %s host port msg\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    const char* host = argv[1];
    const char* portStr = argv[2];

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    struct addrinfo* result;
    if (getaddrinfo(host, portStr, &hints, &result) != 0)
    {
        fatalError("get addr info error");
    }

    int cfd = 0;
    struct addrinfo* rp;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (cfd == -1)
            continue;

        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        printf("connect failed\n");
        close(cfd);
    }

    freeaddrinfo(result);
    if (rp == NULL)
        fatalError("could not connect socket");

    // get local sock name
    // struct sockaddr localAddr;
    // socklen_t addrLen = sizeof(struct sockaddr);
    // int ret = getsockname(cfd, &localAddr, &addrLen);

    // char host[NI_MAXHOST];
    // char service[NI_MAXSERV];
    // ret = getnameinfo((struct sockaddr*)&localAddr, addrLen, host, NI_MAXHOST, service, NI_MAXSERV, 0);
    // if (ret == 0)
    //     printf("client is %s:%s\n", host, service);

    size_t msgLen = strlen(argv[3]);
    if (write(cfd, argv[3], msgLen) != msgLen)
        fatalError("write size != msg length");
    if (write(cfd, "\n", 1) != 1)
        fatalError("write size != msg length");

    char buffer[g_buffSize];
    ssize_t readSize = readLine(cfd, buffer, g_buffSize);
    if (readSize <= 0)
        fatalError("read line had error");

    printf("read buffer from server: %d [%s]\n", (int)readSize, buffer);

    close(cfd);
    return 0;
}