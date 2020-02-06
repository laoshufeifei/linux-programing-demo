#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define BACKLOG 5

const size_t g_buffSize = 64;
const char* g_socketName = "/tmp/domain_stream_socket";

static void fatalError(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}

int main()
{
    if (remove(g_socketName) == -1 && errno != ENOENT)
        fatalError("remove /tmp/unix_sock error");

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        fatalError("socket sfd == -1");

    socklen_t socketSize = sizeof(struct sockaddr_un);
    struct sockaddr_un addr;
    memset(&addr, 0, socketSize);
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, g_socketName, sizeof(addr.sun_path) - 1);

    int ret = bind(sfd, (struct sockaddr*)&addr, socketSize);
    if (ret == -1)
        fatalError("bind error");

    ret = listen(sfd, BACKLOG);
    if (ret == -1)
        fatalError("listen error");

    printf("wait for new connection\n");
    char buff[g_buffSize];
    for (;;)
    {
        int cfd = accept(sfd, NULL, NULL);
        if (cfd == -1)
            fatalError("accept return -1");

        printf("accept new connetion, cfd = %d\n", cfd);
        ssize_t readSize = 0;
        while ((readSize = read(cfd, buff, g_buffSize)) > 0)
        {
            if (write(STDOUT_FILENO, buff, readSize) != readSize)
            {
                fatalError("write error");
            }
        }

        if (readSize == -1)
            fatalError("read error");

        printf("close cfd\n");
        if (close(cfd) == -1)
            fatalError("close error");
    }

    close(sfd);
    return 0;
}
