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

int main(int argc, char* argv[])
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printf("usage: %s port\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    const char* portStr = argv[1];

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Current working dir: %s\n", cwd);

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        fatalError("signal ignore error");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    struct addrinfo* result;
    int ret = getaddrinfo(NULL, portStr, &hints, &result);
    if (ret != 0)
        fatalError("get addr info error");

    int lfd = 0;
    int optval = 1;
    struct addrinfo* rp;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            continue;

        ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        if (ret == -1)
            fatalError("set sock opt error");

        ret = bind(lfd, rp->ai_addr, rp->ai_addrlen);
        if (ret == 0)
            break;  // success

        printf("bind failed, clost this socket and try next address");
        close(lfd);
    }

    freeaddrinfo(result);
    if (rp == NULL)
        fatalError("couldn't bind socket to any address");

    if (listen(lfd, BACKLOG) == -1)
        fatalError("listen error");

    struct sockaddr localAddr;
    socklen_t addrLen1 = sizeof(struct sockaddr);
    int ret1 = getsockname(lfd, &localAddr, &addrLen1);
    char host1[NI_MAXHOST];
    char service1[NI_MAXSERV];
    ret1 = getnameinfo((struct sockaddr*)&localAddr, addrLen1, host1, NI_MAXHOST, service1, NI_MAXSERV, 0);
    if (ret == 0)
        printf("server socket is %s:%s\n", host1, service1);



    socklen_t addrLen = sizeof(struct sockaddr_storage);
    int cltFd = 0;
    struct sockaddr_in cltAddr;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    char addrStr[ADDRSTRLEN];
    char buffer[g_buffSize];

    printf("wait for new connect\n");
    for (;;)
    {
        cltFd = accept(lfd, (struct sockaddr*)&cltAddr, &addrLen);
        if (cltFd == -1)
        {
            printf("accept error");
            continue;
        }

        // struct sockaddr localAddr;
        // socklen_t addrLen = sizeof(struct sockaddr);
        // int ret = getsockname(cltFd, &localAddr, &addrLen);
        // char host[NI_MAXHOST];
        // char service[NI_MAXSERV];
        // ret = getnameinfo((struct sockaddr*)&localAddr, addrLen, host, NI_MAXHOST, service, NI_MAXSERV, 0);
        // if (ret == 0)
        //     printf("accept socket is %s:%s\n", host, service);

        ret = getnameinfo((struct sockaddr*)&cltAddr, addrLen, host, NI_MAXHOST, service, NI_MAXSERV, 0);
        if (ret == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");

        ssize_t readSize = readLine(cltFd, buffer, g_buffSize);
        if (readSize <= 0)
        {
            // Failed read; skip request
            close(cltFd);
            continue;
        }
        printf("connection from %s: %d [%s]\n", addrStr, (int)readSize, buffer);

        if (write(cltFd, buffer, strlen(buffer)) != readSize)
            fprintf(stderr, "Error on write");

        if (close(cltFd) == -1)
            printf("close error");
    }
}
