#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

typedef long long int64;
typedef unsigned long long uint64;

int main(void)
{
    struct timeval tv = {0, 0};
    int ret = gettimeofday(&tv, NULL);
    if (ret == -1)
    {
        printf("gettimeofday return -1, errno is %d\n", errno);
        return 0;
    }

    // must use root
    tv.tv_sec += 600;
    ret = settimeofday(&tv, NULL);
    if (ret == -1)
    {
        printf("settimeofday return -1, errno is %d\n", errno);
        return 0;
    }
    printf("call settimeofday ok\n");


    // adjtime does not work at my ubuntu
    // return -1 and errno is 38 at WSL(2019.09.21 version1)
    struct timeval delta = {-600, 0};
    struct timeval oldTime = {0, 0};
    ret = adjtime(&delta, &oldTime);
    if (ret == -1)
    {
        printf("adjtime return -1, errno is %d\n", errno);
        return -1;
    }
    printf("call adjtime ok\n");

    return 0;
}
