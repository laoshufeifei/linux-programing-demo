#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

typedef unsigned long long uint64;

static int g_timerValueSec = 3;			// 首次到期时间(秒，下同)
static int g_timerIntervalSec = 3;		// 间隔时间
static int g_maxCallbackCount = 10;		// 最多执行几次回调(退出条件)

static uint64 g_initTime = 0;
static uint64 _getTicks()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return (uint64)(tp.tv_sec * 1000) + (uint64)(tp.tv_nsec / 1000000. + 0.5);
}

static double _getInterval()
{
	return (_getTicks() - g_initTime) / 1000.;
}

static uint64 timespec2MS(struct timespec* tp)
{
	return (uint64)tp->tv_sec * 1000 + (uint64)(tp->tv_nsec / 1000000. + 0.5);
}

int main(void)
{
	g_initTime = _getTicks();

	int timerFd = timerfd_create(CLOCK_REALTIME, 0);
	if (timerFd < 0)
	{
		printf("timerfd_create failed, errno: %d\n", errno);
		return -1;
	}

	struct itimerspec ts;
	ts.it_value.tv_sec = g_timerValueSec;
	ts.it_value.tv_nsec = 0;
	ts.it_interval.tv_sec = g_timerIntervalSec;
	ts.it_interval.tv_nsec = 0;
	timerfd_settime(timerFd, 0, &ts, NULL);

	int epollFd = epoll_create(1);
	struct epoll_event ev;
	ev.data.fd = timerFd;
	ev.events = EPOLLIN;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev);

	int eventNumber = 1;
	struct epoll_event events[eventNumber];

	int i = 0;
	while (i < g_maxCallbackCount)
	{
		i++;
		int ret = epoll_wait(epollFd, events, eventNumber, 10000);
		switch (ret)
		{
		case -1:
			printf("epoll_wait had error, return is %d, errno is %d\n", ret, errno);
			if (errno == EINTR)
			{
				printf("receive EINTR\n");
				continue;
			}
			break;
		case 0:
			printf("timeout...\n");
			break;
		default:
			for (int i = 0; i < ret; i++)
			{
				int e = events[i].events;
				int fd = events[i].data.fd;
				if (e & EPOLLIN)
				{
					printf("%.3f timer is ready\n", _getInterval());
					char buffer[512];
					// must read
					size_t readSize = read(fd, buffer, sizeof(u_int64_t));
					if (readSize != sizeof(u_int64_t))
					{
						printf("readSize(%zu) != sizeof(u_int64_t)(%zu)\n", readSize, sizeof(u_int64_t));
						continue;
					}

					struct itimerspec info;
					timerfd_gettime(fd, &info);
					printf("\tit_value: %lld\n", timespec2MS(&info.it_value));
					printf("\tit_interval: %lld\n", timespec2MS(&info.it_interval));
				}
			}
		}
	}

	close(timerFd);
	return 0;
}
