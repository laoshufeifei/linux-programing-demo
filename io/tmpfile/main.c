#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static void _writeAndRead(int fd)
{
    if (fd < 0)
    {
        printf("fd is %d < 0", fd);
        return;
    }

    char buffer[2] = {'a',};
    ssize_t writeSize = write(fd, buffer, 1);
    if (writeSize != 1)
    {
        printf("writeSize(%zd) != 1, errno is %d\n", writeSize, errno);
    }

    lseek(fd, 0, SEEK_SET);
    buffer[0] = 0;
    ssize_t readSize = read(fd, buffer, 1);
    if (readSize != 1)
    {
        printf("readSize(%zd) != 1\n, errno is %d\n", readSize, errno);
    }

    printf("buffer[0] is %c, test %s\n", buffer[0], buffer[0] == 'a' ? "ok" : "failed");
}

int main(void)
{
    char name[] = "tmp-test-XXXXXX";
    int fd1 = mkstemp(name);
    printf("mkstemp(%s) return %d, errno is %d\n", name, fd1, errno);
    unlink(name);
    _writeAndRead(fd1);
    close(fd1);

    FILE* f = tmpfile();
    int fd2 = fileno(f);
    printf("tmpfile return %d, errno is %d\n", fd2, errno);
    _writeAndRead(fd2);
    close(fd2);
}