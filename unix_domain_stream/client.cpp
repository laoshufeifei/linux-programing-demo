#include "header.h"

int main()
{
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        fatalError("socket == -1");

    socklen_t socketSize = sizeof(struct sockaddr_un);
    struct sockaddr_un addr;
    memset(&addr, 0, socketSize);
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, g_socketName, sizeof(addr.sun_path));

    int ret = connect(sfd, (struct sockaddr*)&addr, socketSize);
    if (ret == -1)
        fatalError("connect error");

    char buff[g_buffSize];
    ssize_t readSize = 0;
    while ((readSize = read(STDIN_FILENO, buff, g_buffSize)) > 0)
    {
        if (write(sfd, buff, readSize) != readSize)
        {
            fatalError("write error");
        }
    }

    if (readSize == -1)
        fatalError("readSize == -1");

    exit(EXIT_SUCCESS);
}
