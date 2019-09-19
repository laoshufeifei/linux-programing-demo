#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

typedef long long int64;
typedef unsigned long long uint64;

// unit is ms
uint64 getTimeStamp(void)
{
    struct timeval tv = {0, 0};
    int ret = gettimeofday(&tv, NULL);
    if (ret == -1)
    {
        printf("gettimeofday return -1, errno is %d\n", errno);
        return 0;
    }

    return (uint64)tv.tv_sec * 1000 + (uint64)(tv.tv_usec / 1000);
}

// just for show
uint64 getTimeStamp2(void)
{
    time_t t = time(NULL);
    if (t == (time_t)-1)
    {
        printf("time() return -1, errno is %d\n", errno);
        return 0;
    }

    return (uint64)t * 1000;
}

void showGm(struct tm* t, const char* msg)
{
    printf("%s %04d-%02d-%02d %02d:%02d:%02d week-%d\n", msg,
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec, t->tm_wday + 1);
}

int main(void)
{
    printf("get timestamp...\n");
    uint64 t1 = getTimeStamp();
    uint64 t2 = getTimeStamp2();
    printf("t1 is %llu\n", t1);
    printf("t2 is %llu\n", t2);


    printf("\n\ntest ctime()\n");
    time_t t = t1 / 1000;
    char buffer[64];
    char* ctimeRet = ctime_r(&t, buffer);
    if (ctimeRet == NULL)
    {
        printf("ctime_r return NULL, errno is %d\n", errno);
        return -1;
    }
    printf("%s\n", buffer);


    printf("\n\nconvert time_t to struct tm\n");
    struct tm utcTm;
    struct tm* gmtimeRet = gmtime_r(&t, &utcTm);
    if (gmtimeRet == NULL)
    {
        printf("gmTime_r return NULL, errno is %d\n", errno);
        return -1;
    }
    showGm(&utcTm, "  UTC:");

    struct tm locTm;
    gmtimeRet = localtime_r(&t, &locTm);
    showGm(&locTm, "local:");


    printf("\n\nmktime convert struct tm to time_t\n");
    printf("convet utcTm to time_t is %ld (wrong)\n", mktime(&utcTm));
    printf("convet locTm to time_t is %ld\n", mktime(&locTm));


    return 0;
}