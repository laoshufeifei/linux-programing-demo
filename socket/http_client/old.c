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
#include <stdbool.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

extern int h_errno;

int main(int argc, char* argv[])
{
    const int port = 80;
    const char* host = "liudf.com";
    const char* msg = "HEAD / HTTP/1.1\n"
                        "Host: liudf.com\n"
                        "Accept-Encoding: identity\n"
                        "Transfer-Encoding: identity\n"
                        "\n";

    printf("%s", msg);

    struct sockaddr_in addr;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    in_addr_t addrRet = inet_addr(host);
    if (addrRet != -1)
    {
        printf("inet_addr ok\n");
        addr.sin_addr.s_addr = addrRet;
    }
    else
    {
        struct hostent* ph = gethostbyname(host);
        if (ph == NULL)
        {
            printf("gethostbyname return NULL, errno is %d, h_errno is %d(%s)\n",
                errno, h_errno, hstrerror(h_errno));
            return -1;
        }

        printf("gethostbyname ok\n");
        memcpy(&addr.sin_addr, ph->h_addr, ph->h_length);
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd <= 0)
    {
        printf("socket return <= 0");
        return -1;
    }

   int ret = connect(fd, (const struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        printf("connect return %d, errno is %d\n", ret, errno);
        return -1;
    }

    size_t msgLen = strlen(msg);
    if (write(fd, msg, msgLen) != msgLen)
    {
        printf("write size != msg length");
        return -1;
    }
    printf("\n");

    char ch;
    char buffer[1024] = {0, };
    for (int i = 0; i < 1024; i++)
    {
        ssize_t readSize = read(fd, &ch, 1);
        if (readSize < 0)
            break;
        else if (readSize == 0)
            continue;

        printf("%c", ch);
        buffer[i] = ch;

        if (i >= 3)
        {
            if (strncmp(buffer + i - 3, "\r\n\r\n", 4) == 0)
            {
                break;
            }
        }
    }

    close(fd);
    return 0;
}
