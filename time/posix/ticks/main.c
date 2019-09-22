#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>

typedef long long int64;
typedef unsigned long long uint64;

static uint64 timeval2ms(struct timeval* t)
{
	return (uint64)t->tv_sec * 1000 + (uint64)(t->tv_usec / 1000. + 0.5);
}

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
	usleep(300);
	return 0.;
}

void costTimeTest()
{
	costTime();

	// show static info(usr cpu time + sys cpu time)
	struct timespec tp;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
	printf("CLOCK_PROCESS_CPUTIME_ID(usr + sys) is %llu\n", timespec2MS(&tp));

	clockid_t cId;
	clock_getcpuclockid(0, &cId);
	clock_gettime(cId, &tp);
	printf("clock_getcpuclockid(usr + sys) is %llu\n", timespec2MS(&tp));

	int ret = clock();
	if (ret == (clock_t)-1)
	{
		printf("clock() return -1, errno is %d\n", errno);
		return;
	}
	printf("clock static info(usr + sys) is %ld\n", ret * 1000 / CLOCKS_PER_SEC);

	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	printf("RUSAGE_SELF - ru_utime(usr): %llu\n", timeval2ms(&ru.ru_utime));
	printf("RUSAGE_SELF - ru_stime(sys): %llu\n", timeval2ms(&ru.ru_stime));
}

int main(void)
{
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);

	costTimeTest();

	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("real time cost %llu\n", timeInterval(&start, &end));

	return 0;
}
