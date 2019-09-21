#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

typedef long long int64;
typedef unsigned long long uint64;

static uint64 timespec2MS(struct timespec* tp)
{
	return (uint64)tp->tv_sec * 1000 + (uint64)tp->tv_nsec / 1000000;
}

static int getSetRealTime(void)
{
	struct timespec tp;
	int ret = clock_gettime(CLOCK_REALTIME, &tp);
	if (ret == -1)
	{
		printf("clock_gettime returned -1, errno: %d\n", errno);
		return -1;
	}
	uint64 stamp = timespec2MS(&tp);
	printf("clock_gettime with CLOCK_REALTIME  timestamp is %llu\n", stamp);

	printf("use clock_settime to change sys time");
	tp.tv_sec += 600;

	// must use root
	// return -1 and errno is 38 at WSL(2019.09.21 version1)
	ret = clock_settime(CLOCK_REALTIME, &tp); // must use CLOCK_REALTIME
	if (ret == -1)
	{
		printf("clock_settime returned -1, errno is %d\n", errno);
		return -1;
	}
}

int main(void)
{
	getSetRealTime();

	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	printf("clock_gettime with CLOCK_MONOTONIC timestamp is %llu\n", timespec2MS(&tp));

	clock_getres(CLOCK_REALTIME, &tp);
	printf("clock_getres  with CLOCK_REALTIME  timestamp is %llu\n", timespec2MS(&tp));

	return 0;
}
