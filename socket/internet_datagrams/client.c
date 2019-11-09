#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

const size_t g_buffSize = 64;

static void fatalError(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 4 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printf("usage: %s host port msg1 msg2 msg3...\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    const char* host = argv[1];
    int port = atoi(argv[2]);
    if (port <= 0)
    {
        printf("port number(%s) is illegal\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1)
        fatalError("sfd == -1");

    socklen_t sockLenth = sizeof(struct sockaddr_in);
    struct sockaddr_in svrAddr;
    memset(&svrAddr, 0, sockLenth);
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port = htons(port);
    inet_pton(AF_INET, host, &svrAddr.sin_addr);

    char buffer[g_buffSize];
    for (int i = 3; i < argc; i++)
    {
        int msgLength = strlen(argv[i]);
        int sendSize = sendto(sfd, argv[i], msgLength, 0, (struct sockaddr*)&svrAddr, sockLenth);
        if (sendSize != msgLength)
            fatalError("send error");

        int recvSize = recvfrom(sfd, buffer, g_buffSize, 0, NULL, NULL);
        if (recvSize == -1)
            fatalError("recvSize == -1");

        printf("response %d: %.*s\n", i - 1, (int)recvSize, buffer);
    }

    exit(EXIT_SUCCESS);
}
