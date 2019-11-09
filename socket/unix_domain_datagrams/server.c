#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

const size_t g_buffSize = 64;
const char* g_socketName = "/tmp/domain_datagrams_socket";

static void fatalError(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}

int main()
{
    if (remove(g_socketName) == -1 && errno != ENOENT)
        fatalError("remove /tmp/unix_sock error");

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        fatalError("socket sfd == -1");

    // server address
    struct sockaddr_un svrAddr;
    socklen_t socketSize = sizeof(struct sockaddr_un);
    memset(&svrAddr, 0, socketSize);
    svrAddr.sun_family = AF_UNIX;
    strncpy(svrAddr.sun_path, g_socketName, sizeof(svrAddr.sun_path) - 1);

    int ret = bind(sfd, (struct sockaddr*)&svrAddr, socketSize);
    if (ret == -1)
        fatalError("bind error");

    // no listen
    printf("wait for new connection\n");

    char buff[g_buffSize];
    struct sockaddr_un cltAddr;
    int recvSiz = 0;
    for (;;)
    {
        recvSiz = recvfrom(sfd, buff, g_buffSize, 0, (struct sockaddr*)&cltAddr, &socketSize);
        if (recvSiz == -1)
            fatalError("recvSiz == -1");

        printf("server received %d bytes(%.*s) from %s\n", recvSiz, recvSiz, buff, cltAddr.sun_path);
        if (sendto(sfd, buff, recvSiz, 0, (struct sockaddr*)&cltAddr, socketSize) != recvSiz)
            fatalError("sendto had error");
    }

    return 0;
}
