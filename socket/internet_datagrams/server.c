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
const int g_port = 20000;

static void fatalError(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}

int main()
{
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1)
        fatalError("sfd == -1");

    socklen_t sockLenth = sizeof(struct sockaddr_in);
    struct sockaddr_in svrAddr;
    memset(&svrAddr, 0, sockLenth);
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svrAddr.sin_port = htons(g_port);

    int ret = bind(sfd, (struct sockaddr*)&svrAddr, sockLenth);
    if (ret == -1)
        fatalError("bind error");

    char buffer[g_buffSize];
    char cltAddrStr[INET_ADDRSTRLEN];
    struct sockaddr_in cltAddr;
    printf("wait for new connect\n");
    for (;;)
    {
        int recvSize = recvfrom(sfd, buffer, g_buffSize, 0, (struct sockaddr*)&cltAddr, &sockLenth);
        if (recvSize == -1)
            fatalError("recvSize == -1");

        const char* p = inet_ntop(AF_INET, &cltAddr.sin_addr, cltAddrStr, INET_ADDRSTRLEN);
        if (p == NULL)
        {
            printf("couldn't convet client address to string\n");
        }
        else
        {
            printf("server received %ld bytes(%.*s) from (%s:%u)\n", (long)recvSize, recvSize, buffer, cltAddrStr, ntohs(cltAddr.sin_port));
        }

        int sendSize = sendto(sfd, buffer, recvSize, 0, (struct sockaddr*)&cltAddr, sockLenth);
        if (sendSize != recvSize)
        {
            fatalError("send error");
        }
    }
}
