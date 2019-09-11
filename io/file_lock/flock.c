#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

static int _otherTest(const char* fileName, bool testDup, bool testClose)
{
    printf("\n\n");
    int fd1 = open(fileName, O_RDWR);
    int fd2 = open(fileName, O_RDWR);
    if (fd1 < 0 || fd2 < 0)
    {
        printf("fd1 or fd2 < 0\n");
        return -1;
    }

    printf("open %s twice, fd1 = %d, fd2 = %d\n", fileName, fd1, fd2);
    printf("try flock fd1\n");
    int ret = flock(fd1, LOCK_EX | LOCK_NB);
    if (ret == -1)
    {
        printf("flock fd2 failed, errno is %d\n", errno);
        close(fd1);
        close(fd2);
        return -1;
    }
    else
    {
        printf("flock fd1 ok\n");
    }

    int fd3 = -1;
    if (testDup)
    {
        fd3 = dup(fd1);
        printf("    dup fd1 to fd3(%d)\n", fd3);

        flock(fd3, LOCK_UN);
        printf("    unlock fd3 will unlock fd1 too\n");
    }

    if (testClose)
    {
        close(fd1);
        fd1 = -1;
        printf("    close fd1, will not unlock fd1\n");
    }

    printf("try flock fd2\n");
    ret = flock(fd2, LOCK_EX | LOCK_NB);
    if (ret == -1)
    {
        printf("flock fd2 failed, errno is %d\n", errno);
    }
    else
    {
        printf("flock fd2 ok\n");
    }

    if (fd1 >= 0)
        close(fd1);
    if (fd2 >= 0)
        close(fd2);
    if (fd3 >= 0)
        close(fd3);
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 3 || strcmp(argv[1], "--help") == 0 || strchr("sx", argv[2][0]) == NULL)
    {
        printf("Usage: %s file s[n]/x[n] [sleep time]\n"
            "	s: shared lock\n"
            "	x: exclusive lock\n"
            "	n: optionally; nonblocking\n"
            "	sleep-time: specifies time to hold lock; if net set is 10 s\n"
            , argv[0]);
        return -1;
    }
    printf("LOCK_SH = %d, LOCK_EX = %d, LOCK_NB = %d\n", LOCK_SH, LOCK_EX, LOCK_NB);

    const char* fileName = argv[1];
    int lockType = argv[2][0] == 's' ? LOCK_SH : LOCK_EX;
    if (argv[2][1] == 'n')
        lockType |= LOCK_NB;
    int sleepTime = 10;
    if (argc > 3)
        sleepTime = atoi(argv[3]);

    int fd = open(fileName, O_RDONLY);
    if (fd == -1)
    {
        printf("open %s failed\n", fileName);
        return -1;
    }

    long pid = (long)getpid();
    printf("pid: %ld requesting lock %s %d s, lock type is %d\n", pid, fileName, sleepTime, lockType);

    if(flock(fd, lockType) == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            printf("pid: %ld already been locked by other process\n", pid);
            return -1;
        }
        else
        {
            printf("pid: %ld flock %s return -1\n", pid, fileName);
            return -1;
        }
    }

    printf("pid: %ld granted lock %s, lock type is %d\n", pid, fileName, lockType);
    sleep(sleepTime);
    printf("pid: %ld release lock %s, lock type is %d\n", pid, fileName, lockType);

    if (flock(fd, LOCK_UN) == -1)
    {
        printf("flock un lock return -1, errno is %d\n", errno);
        return -1;
    }
    close(fd);

    _otherTest(argv[1], false, false);
    _otherTest(argv[1], true, false);
    _otherTest(argv[1], false, true);

    return 0;
}