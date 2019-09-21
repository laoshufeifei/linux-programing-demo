#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/times.h>

typedef long long int64;
typedef unsigned long long uint64;
static int g_clockTick = 0;

static uint64 timespec2MS(struct timespec* tp)
{
    return (uint64)tp->tv_sec * 1000 + (uint64)(tp->tv_nsec / 1000000. + 0.5);
}

static uint64 timeInterval(struct timespec* tp1, struct timespec* tp2)
{
    return (uint64)(tp2->tv_sec - tp1->tv_sec) * 1000 + (uint64)((tp2->tv_nsec - tp1->tv_nsec) / 1000000. + 0.5);
}

static double costTime(void)
{
    // cost user cpu time
    for (int i = 0; i < 1000000; i++)
    {
        double d1 = 1.0 * i / 1000. * 3.1415926;
        double d2 = (i + i / 2. * 1.0) * 37.;
        d1 *= d2;
        d2 += d1;
        d2 *= d1;
    }

    // cost sys cpu time
    for (int i = 0; i < 1000000; i++)
    {
        getppid();
    }

    // only cost real time
    sleep(1);
    return 0.;
}

void costTimeTest()
{
    costTime();

    // show static info(usr cpu time + sys cpu time)
    printf("use times() static info\n");
    struct tms ts;
    clock_t ret = times(&ts);
    if (ret == (clock_t)-1)
    {
        printf("times return -1, errno is %d\n", errno);
        return;
    }
    printf("utime  is %ld ms\n", ts.tms_utime  * 1000 / g_clockTick);
    printf("stime  is %ld ms\n", ts.tms_stime  * 1000 / g_clockTick);
    printf("cutime is %ld ms\n", ts.tms_cutime * 1000 / g_clockTick);
    printf("cstime is %ld ms\n", ts.tms_cstime * 1000 / g_clockTick);


    printf("\n\nuse clock() static info\n");
    ret = clock();
    if (ret == (clock_t)-1)
    {
        printf("times return -1, errno is %d\n", errno);
        return;
    }
    printf("clock static info(usr + sys) is %ld\n", ret * 1000 / CLOCKS_PER_SEC);
}

int main(void)
{
    g_clockTick = sysconf(_SC_CLK_TCK);
    costTimeTest();

    return 0;
}
