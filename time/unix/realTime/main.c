#define _XOPEN_SOURCE // for strptime
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

typedef long long int64;
typedef unsigned long long uint64;

// unit is ms
static uint64 getTimeStamp(void)
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
static uint64 getTimeStamp2(void)
{
    time_t t = time(NULL);
    if (t == (time_t)-1)
    {
        printf("time() return -1, errno is %d\n", errno);
        return 0;
    }

    return (uint64)t * 1000;
}

static void showStructTm(struct tm* t)
{
    printf("%04d-%02d-%02d %02d:%02d:%02d week-%d(Sunday is 0)\n",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec, t->tm_wday);
}

int main(void)
{
    printf("gettimeofday() and time() get timestamp(ms)...\n");
    uint64 t1 = getTimeStamp();
    uint64 t2 = getTimeStamp2();
    printf("t1 is %llu\n", t1);
    printf("t2 is %llu\n", t2);


    printf("\n\nctime_r(): convet timer_t to string\n");
    time_t t = t1 / 1000;
    char buffer[64];
    char* ctimeRet = ctime_r(&t, buffer);
    if (ctimeRet == NULL)
    {
        printf("ctime_r return NULL, errno is %d\n", errno);
        return -1;
    }
    printf("%s", buffer);   // buffer is end of '\n'


    printf("\n\ngmtime_r(): convert time_t to struct tm(UTC)\n");
    struct tm utcTm;
    struct tm* gmtimeRet = gmtime_r(&t, &utcTm);
    if (gmtimeRet == NULL)
    {
        printf("gmtime_r return NULL, errno is %d\n", errno);
        return -1;
    }
    showStructTm(&utcTm);

    printf("asctime_r(): convert struct tm to string\n");
    asctime_r(&utcTm, buffer);        // ignore check return
    printf("UTC  : %s", buffer);      // buffer is end of '\n'


    printf("\n\nlocaltime_r(): convert time_t to struct tm(local)\n");
    struct tm locTm;
    gmtimeRet = localtime_r(&t, &locTm);
    showStructTm(&locTm);

    printf("asctime_r(): convert struct tm to string\n");
    asctime_r(&locTm, buffer);      // ignore check return
    printf("local: %s", buffer);    // buffer is end of '\n'


    printf("\n\nmktime(): convert struct tm to time_t\n");
    printf("convet utcTm to time_t is %ld (wrong)\n", mktime(&utcTm));
    printf("convet locTm to time_t is %ld\n", mktime(&locTm));


    printf("\n\nstrftime() format struct tm\n");
    // %w[0-6]: Sunday is 0; %u[1-7]: Monday is 1;
    size_t retSize = strftime(buffer, 64, "%F %T week-%u(Monday is 1)", &utcTm);
    if (retSize == 0)
    {
        printf("strftime return 0, errno is %d\n", errno);
        return -1;
    }
    printf("format utcTm: %s\n", buffer);

    strftime(buffer, 64, "%F %T week-%u(Monday is 1)", &locTm);
    printf("format locTm: %s\n", buffer);


    // Sat, 21 Sep 2019 06:15:46 GMT
    printf("\n\nstrftime() format now time to http date\n");
    strftime(buffer, 64, "%a, %d %b %Y %H:%M:%S GMT", &utcTm);
    printf("format utcTm to http date: %s\n", buffer);


    // should #define _XOPEN_SOURCE
    printf("\n\nstrftime() convet http date to struct tm\n");
    struct tm tmpTm;
    strptime(buffer, "%a, %d %b %Y %H:%M:%S GMT", &tmpTm);
    showStructTm(&tmpTm);

    return 0;
}