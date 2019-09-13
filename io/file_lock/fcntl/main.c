#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

static bool g_testTwice = true;
static bool g_testClose = false;

static int _lock(int fd, char c, char t, char w, int start, off_t len)
{
    long pid = (long)getpid();

    int cmd = 0;
    char* cmdStr = "";
    switch(c)
    {
    case 'g': cmd = F_GETLK; cmdStr = "F_GETLK"; break;
    case 's': cmd = F_SETLK; cmdStr = "F_SETLK"; break;
    case 'w': cmd = F_SETLKW; cmdStr = "F_SETLKW"; break;
    default: printf("unknow cmd %c\n", c); return -1;
    }

    int lockType = 0;
    char* lockTypeStr = "";
    switch(t)
    {
    case 'r': lockType = F_RDLCK; lockTypeStr = "F_RDLCK"; break;
    case 'w': lockType = F_WRLCK; lockTypeStr = "F_WRLCK"; break;
    case 'u': lockType = F_UNLCK; lockTypeStr = "F_UNLCK"; break;
    default: printf("unknow lockType %c\n", t); return -1;
    }

    int whence = SEEK_SET;
    char* whenceStr = "SEEK_SET";
    switch (w)
    {
    case 'c': whence = SEEK_CUR; whenceStr = "SEEK_CUR"; break;
    case 'e': whence = SEEK_END; whenceStr = "SEEK_END"; break;
    default: break;
    }

    int fd2 = dup(fd);
    long length = (long)len;
    // printf("pid: %ld will fcntl: %s-%s for %d: %s[%d+%ld]\n",
        // pid, cmdStr, lockTypeStr, fd, whenceStr, start, length);

    struct flock fl;
    fl.l_type = lockType;
    fl.l_whence = whence;
    fl.l_start = start;
    fl.l_len = len;
    int ret = fcntl(fd, cmd, &fl);

    int result = -1;
    if (cmd == F_GETLK)
    {
        if (ret == -1)
        {
            printf("fcntl - F_GETLK return error, errno is %d\n", errno);
        }
        else if (fl.l_type == F_UNLCK)
        {
            // ok
            result = 0;
            printf("pid: %ld fcntl: %s-%s for %d: %s[%d+%ld] succeeded\n",
                pid, cmdStr, lockTypeStr, fd, whenceStr, start, length);
        }
        else
        {
            printf("pid: %ld fcntl: %s-%s for %d: %s[%d+%ld] denied(by pid: %ld)\n",
                pid, cmdStr, lockTypeStr, fd, whenceStr, start, length, (long)(fl.l_pid));
        }
    }
    else
    {
        // F_SETLK/F_SETLKW
        if (ret == 0)
        {
            // ok
            result = 0;
            printf("pid: %ld fcntl: %s-%s for %d: %s[%d+%ld] succeeded\n",
                pid, cmdStr, lockTypeStr, fd, whenceStr, start, length);
        }
        else if (errno == EAGAIN || errno == EACCES)
        {
            printf("pid: %ld fcntl: %s-%s for %d: %s[%d+%ld] failed\n",
                pid, cmdStr, lockTypeStr, fd, whenceStr, start, length);
        }
        else if (errno == EDEADLK)
        {
            printf("pid: %ld fcntl: %s-%s for %d: %s[%d+%ld] deaklock\n",
                pid, cmdStr, lockTypeStr, fd, whenceStr, start, length);
        }
        else
        {
            printf("fcntl - F_SETLK(w) return error, errno is %d\n", errno);
        }
    }

    // test close duplicate
    if (g_testClose && result == 0)
    {
        sleep(5);
        close(fd2);
        printf("close fd2\n");
        result = -1;
    }

    return result;
}

static int _otherTest(const char* fileName)
{
    printf("open file twice test begin...\n");
    int fd1 = open(fileName, O_RDWR);
    int fd2 = open(fileName, O_RDWR);
    if (fd1 == -1 || fd2 == -1)
    {
        printf("open %s failed, errno is %d\n", fileName, errno);
        return -1;
    }

    _lock(fd1, 's', 'w', 's', 0, 10);
    _lock(fd2, 's', 'w', 's', 0, 10);

    printf("open file twice test finish...\n");
    return 0;
}

int main(int argc, char* argv[])
{
    if (g_testTwice && argc > 1)
    {
        _otherTest(argv[1]);
    }

    if (argc < 6 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s file cmd lock start length [whence] [sleep time]\n"
            "   file: file name for test lock\n"
            "   cmd: \n"
            "       g: F_GETLK\n"
            "       s: F_SETLK\n"
            "       w: F_SETLKW\n"
            "   lock: \n"
            "       r: F_RDLCK\n"
            "       w: F_WRLCK\n"
            "       u: F_UNLCK\n"
            "   start and length specify bytes range to lock\n"
            "   whence: \n"
            "       s: SEEK_SET (if net set is SEEK_SET)\n"
            "       c: SEEK_CUR\n"
            "       e: SEEK_END\n"
            "   sleep-time: specifies time to hold lock; if net set is 10 s\n"
            , argv[0]);
        return -1;
    }

    const char* fileName = argv[1];
    // if open with O_RDONLY, can only set F_RDLCK
    // if open with O_WRONLY, can only set F_WRLCK
    int fd = open(fileName, O_RDWR);
    if (fd == -1)
    {
        printf("open %s failed, errno is %d\n", fileName, errno);
        return -1;
    }

    char c = argv[2][0];
    char t = argv[3][0];
    int start = atoi(argv[4]);
    int length = atoi(argv[5]);
    if (start == 0 && length == 0)
    {
        printf("start and length all equal 0\n");
        return -1;
    }

    char w = 's';
    if (argc > 6)
        c = argv[6][0];

    int sleepTime = 10;
    if (argc > 7)
        sleepTime = atoi(argv[7]);

    if (_lock(fd, c, t, w, start, length) == 0)
    {
        sleep(sleepTime);
    }

    close(fd);
    return 0;
}
