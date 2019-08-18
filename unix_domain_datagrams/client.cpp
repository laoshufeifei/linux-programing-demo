#include "header.h"

int main(int argc, char* argv[])
{
    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        fatalError("socket == -1");

    socklen_t socketSize = sizeof(struct sockaddr_un);
    struct sockaddr_un cltAddr;
    memset(&cltAddr, 0, socketSize);
    cltAddr.sun_family = AF_UNIX;
    snprintf(cltAddr.sun_path, sizeof(cltAddr.sun_path), "/tmp/ud_cl.%ld", (long)getpid());
    printf("cltAddr.sun_path is %s\n", cltAddr.sun_path);

    int ret = bind(sfd, (struct sockaddr*)&cltAddr, socketSize);
    if (ret == -1)
        fatalError("bind error");

    // server address
    struct sockaddr_un svrAddr;
    memset(&svrAddr, 0, socketSize);
    svrAddr.sun_family = AF_UNIX;
    strncpy(svrAddr.sun_path, g_socketName, sizeof(svrAddr.sun_path) - 1);

    char buff[g_buffSize];
    int msgLen = 0;
    ssize_t transferLen = 0;
    for (int i = 1; i < argc; i++)
    {
        msgLen = strlen(argv[i]);
        transferLen = sendto(sfd, argv[i], msgLen, 0, (struct sockaddr*)&svrAddr, socketSize);
        if (transferLen != msgLen)
            fatalError("sendto error");

        transferLen = recvfrom(sfd, buff, g_buffSize, 0, NULL, NULL);
        if (transferLen == -1)
            fatalError("recvfrom error");

        printf("response %d: %.*s\n", i, (int)transferLen, buff);
    }

    remove(cltAddr.sun_path);
    exit(EXIT_SUCCESS);
}
