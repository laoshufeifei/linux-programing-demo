#include "common.h"

int main(int argc, char* argv[])
{
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        fatalError("signal ignore error");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    struct addrinfo* result;
    int ret = getaddrinfo(NULL, g_portStr, &hints, &result);
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

    if (rp == NULL)
        fatalError("couldn't bind socket to any address");

    if (listen(lfd, BACKLOG) == -1)
        fatalError("listen error");

    freeaddrinfo(result);


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

        ret = getnameinfo((struct sockaddr*)&cltAddr, addrLen, host, NI_MAXHOST, service, NI_MAXSERV, 0);
        if (ret == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        printf("connection from %s\n", addrStr);

        ssize_t readSize = readLine(cltFd, buffer, g_buffSize);
        if (readSize <= 0)
        {
            // Failed read; skip request
            close(cltFd);
            continue;
        }

        if (write(cltFd, buffer, strlen(buffer)) != readSize)
            fprintf(stderr, "Error on write");

        if (close(cltFd) == -1)
            printf("close error");
    }
}
