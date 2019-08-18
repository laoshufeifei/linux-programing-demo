#include "header.h"

int main(int argc, char *argv[])
{
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
    {
        printf("usage: %s host-address msg...\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1)
        fatalError("sfd == -1");

    socklen_t sockLenth = sizeof(struct sockaddr_in);
    struct sockaddr_in svrAddr;
    memset(&svrAddr, 0, sockLenth);
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port = htons(g_portNum);
    inet_pton(AF_INET, argv[1], &svrAddr.sin_addr);

    char buffer[g_buffSize];
    for (int i = 2; i < argc; i++)
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
