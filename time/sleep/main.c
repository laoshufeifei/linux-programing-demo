#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <stdbool.h>

typedef long long int64;
typedef unsigned long long uint64;

typedef enum SleepType
{
	SleepType_sleep = 0,
	SleepType_usleep,
	SleepType_nanosleep,
	SleepType_select,
	SleepType_poll,
	SleepType_epoll,
	SleepType_posixClock_abs,
	SleepType_posixClock_relative,
	SleepType_invalid
} SleepType;

const char* sleepType2String(SleepType t)
{
	switch (t)
	{
	case SleepType_sleep:                   return "sleep";
	case SleepType_usleep:                  return "usleep";
	case SleepType_nanosleep:               return "nanosleep";
	case SleepType_select:                  return "select";
	case SleepType_poll:                    return "poll";
	case SleepType_epoll:                   return "epoll";
	case SleepType_posixClock_abs:          return "posixClock_abs";
	case SleepType_posixClock_relative:     return "posixClock_relative";
	default:                                break;
	}
}

static uint64 timespec2ms(struct timespec* t)
{
	return (uint64)t->tv_sec * 1000 + (uint64)(t->tv_nsec / 1000000. + 0.5 );
}

static uint64 timeInterval(struct timespec* tp1, struct timespec* tp2)
{
	return (uint64)(tp2->tv_sec - tp1->tv_sec) * 1000 + (uint64)((tp2->tv_nsec - tp1->tv_nsec) / 1000000. + 0.5);
}

static void _checkPosixVersion(void)
{
#if !defined(_POSIX_C_SOURCE)
	printf("not defined _POSIX_C_SOURCE\n");
#elif _POSIX_C_SOURCE < 199309L
	printf("_POSIX_C_SOURCE < 199309L, can't support nanosleep\n");
#elif _POSIX_C_SOURCE < 200112L
	printf("_POSIX_C_SOURCE < 200112L, can't support clock_nanosleep\n");
#else
	printf("_POSIX_C_SOURCE = %lu\n", _POSIX_C_SOURCE);
#endif /* _POSIX_C_SOURCE */
}

void mySleep(uint64 ms, SleepType type)
{
	int ret = 0;
	printf("\n\nwill use [%10s] to sleep %llu ms\n", sleepType2String(type), ms);

	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);
	switch (type)
	{
	case SleepType_sleep:
	{
		while ((ret = sleep(ms / 1000)) > 0)
		{
			printf("ret = %d, errno = %d(%s)\n", ret, errno, strerror(errno));
			printf("remained time: %d s\n", ret);
		}
		break;
	}
	case SleepType_usleep:
	{
		ret = usleep(ms * 1000);
		break;
	}
	case SleepType_nanosleep:
	{
		struct timespec nanoRequest = { 0, 0 };
		nanoRequest.tv_sec = ms / 1000;
		nanoRequest.tv_nsec = (ms % 1000) * 1000000L;
		while ((ret = nanosleep(&nanoRequest, &nanoRequest)) == -1 && errno == EINTR)
		{
			/// interrupt by signal
			printf("ret = %d, errno = %d(%s)\n", ret, errno, strerror(errno));
			printf("remained time: %llu ms\n", timespec2ms(&nanoRequest));
			/// when nanosleep run again and succeed, errno won't be set 0
			errno = 0;
		}
		break;
	}
	case SleepType_select:
	{
		struct timeval selectTime = { 0, 0 };
		selectTime.tv_sec = ms / 1000;
		selectTime.tv_usec = (ms % 1000) * 1000;
		ret = select(0, NULL, NULL, NULL, &selectTime);
		break;
	}
	case SleepType_poll:
	{
		ret = poll(NULL, 0, ms);
		break;
	}
	case SleepType_epoll:
	{
		int fd = epoll_create(1);
		ret = epoll_wait(fd, NULL, 1, ms);
		break;
	}
	case SleepType_posixClock_abs:
	{
		struct timespec nanoRequest = { 0, 0 };
		clock_gettime(CLOCK_MONOTONIC, &nanoRequest);
		nanoRequest.tv_sec += ms / 1000;
		nanoRequest.tv_nsec += (ms % 1000) * 1000000L;
		if (nanoRequest.tv_nsec >= 1000000000L)
		{
			nanoRequest.tv_sec += 1;
			nanoRequest.tv_nsec -= 1000000000L;
		}

		while ((ret = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &nanoRequest, NULL)) == EINTR)
			printf("ret = %d, errno = %s\n", ret, strerror(ret));

		break;
	}
	case SleepType_posixClock_relative:
	{
		struct timespec nanoRequest = { 0, 0 };
		nanoRequest.tv_sec = ms / 1000;
		nanoRequest.tv_nsec = (ms % 1000) * 1000000L;

		while ((ret = clock_nanosleep(CLOCK_MONOTONIC, 0, &nanoRequest, &nanoRequest)) == EINTR)
		{
			printf("ret = %d(%s), remain %llu ms\n", ret, strerror(ret), timespec2ms(&nanoRequest));
		}
		break;
	}
	default:
		printf("unknown sleep type: %d\n", type);
		break;
	}

	struct timespec end;
	clock_gettime(CLOCK_MONOTONIC, &end);
	uint64 cost = timeInterval(&start, &end);
	printf("\tsleep finish, cost: %llu ms\n", cost);

	if (ret != 0)
	{
		printf("ret = %d, errno = %d(%s)\n", ret, errno, strerror(ret));
	}
}

static void _catchSignal(int sig)
{
	printf("catch signal %d \n", sig);
}

int main(int argc, char *argv[])
{
	uint64 sleepTime = 1100;
	if (argc == 2)
		sleepTime = atoi(argv[1]);

	signal(SIGINT, _catchSignal);
	_checkPosixVersion();

	for (int i = 0; i < SleepType_invalid; i++)
	{
		mySleep(sleepTime, i);
	}

	return 0;
}
