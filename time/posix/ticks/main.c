#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

typedef long long int64;
typedef unsigned long long uint64;

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
	struct timespec tp;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
	printf("CLOCK_PROCESS_CPUTIME_ID(usr + sys cpu time) is %llu\n", timespec2MS(&tp));

	clockid_t cId;
	clock_getcpuclockid(0, &cId);
	clock_gettime(cId, &tp);
	printf("clock_getcpuclockid is %llu\n", timespec2MS(&tp));

	printf("if you want static sys cpu time, please use times(), see unix/sticks\n");
}

int main(void)
{
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);
	// printf("clock_gettime with CLOCK_MONOTONIC timestamp is %llu\n", timespec2MS(&start));

	costTimeTest();

	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("real time cost %llu\n", timeInterval(&start, &end));

	return 0;
}
