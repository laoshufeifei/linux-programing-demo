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


int main(int argc, char *argv[])
{
    const char* host = "liudf.com";
    const char* port = "80";
    const char* requestMsg = "HEAD / HTTP/1.1\n"
                            "Host: liudf.com\n"
                            "Accept-Encoding: identity\n"
                            "Transfer-Encoding: identity\n"
                            "\n";

    printf("%s", requestMsg);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    struct addrinfo* result;
    if (getaddrinfo(host, port, &hints, &result) != 0)
    {
        printf("get addr info error\n");
        return -1;
    }

    int fd = 0;
    struct addrinfo* rp;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1)
            continue;

        if (connect(fd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        printf("connect failed\n");
        close(fd);
    }

    freeaddrinfo(result);
    if (rp == NULL)
    {
        printf("could not connect socket");
        return -1;
    }

    size_t msgLen = strlen(requestMsg);
    if (write(fd, requestMsg, msgLen) != msgLen)
    {
        printf("write size != requestMsg length");
        return -1;
    }

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

