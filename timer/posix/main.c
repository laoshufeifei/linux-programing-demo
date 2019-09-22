#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/syscall.h>


static int g_timerValueSec = 2;			// 首次到期时间(秒，下同)
static int g_timerIntervalSec = 2;		// 间隔时间
static int g_callbackCostTime = 5;		// 回调中处理时间
static int g_maxCallbackCount = 10;		// 最多执行几次回调(退出条件)
static int g_callbackCount = 0;			// 用来计数

typedef unsigned long long uint64;
#define gettid() syscall(__NR_gettid)

static uint64 timespec2MS(struct timespec* tp)
{
	return (uint64)tp->tv_sec * 1000 + (uint64)(tp->tv_nsec / 1000000. + 0.5);
}

static void _timerCallback(union sigval sv)
{
	timer_t timerId = *(timer_t*)sv.sival_ptr;
	int tid = (int)gettid();
	printf("\t%d: start  timer callback\n", tid);

	struct itimerspec ts;
	int ret = timer_gettime(timerId, &ts);
	if (ret == -1)
	{
		printf("\ttimer_gettime failed, errno: %d\n", errno);
	}
	else
	{
		printf("\tit_value: %lld\n", timespec2MS(&ts.it_value));
		printf("\tit_interval: %lld\n", timespec2MS(&ts.it_interval));
	}

	ret = timer_getoverrun(timerId);
	if (ret == -1)
	{
		printf("\ttimer_getoverrun failed, errno: %d\n", errno);
	}
	else
	{
		printf("\ttimer_getoverrun return %d\n", ret);
	}

	sleep(g_callbackCostTime);
	g_callbackCount++;

	printf("\t\t%d: finish timer callback\n", tid);
}

// gcc main.c -lrt
int main(void)
{
	int tid = (int)gettid();
	printf("main tid is %d\n", tid);

	timer_t timerId = 0;
	struct sigevent sev;
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = _timerCallback;
	sev.sigev_notify_attributes = NULL;
	sev.sigev_value.sival_ptr = &timerId;

	int ret = timer_create(CLOCK_REALTIME, &sev, &timerId);
	if (ret == -1)
	{
		printf("timer_create returned -1, errno is %d\n", errno);
		return -1;
	}

	struct itimerspec ts;
	ts.it_value.tv_sec = g_timerValueSec;
	ts.it_value.tv_nsec = 0;
	ts.it_interval.tv_sec = g_timerIntervalSec;
	ts.it_interval.tv_nsec = 0;
	// if flag is 0, ts is relative time;
	// if flag is TIMER_ABSTIME, ts is absolute time
	ret = timer_settime(timerId, 0, &ts, NULL);
	if (ret == -1)
	{
		printf("timer_settime failed, errno is %d\n", errno);
		return -1;
	}

	while (g_callbackCount < g_maxCallbackCount)
	{
		sleep(1);
	}

	timer_delete(timerId);
	return 0;
}
