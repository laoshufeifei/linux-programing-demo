#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>

int main(void)
{
	int pipeFds[2] = {-1, -1};
	pipe(pipeFds);
	write(pipeFds[1], "abcd", 4);

	// Since Linux 2.6.8, the size argument is ignored, but must be greater than zero;
	int epollFd = epoll_create(1);
	if (epollFd < 0)
		printf("epoll create return < 0");

	struct epoll_event ev;
	ev.data.fd = STDIN_FILENO;
	ev.events = EPOLLIN; // EPOLLET (edge-triggered) flag
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
		printf("epoll ctl return -1");

	ev.data.fd = pipeFds[0];
	ev.events = EPOLLIN;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
		printf("epoll ctl return -1");

	int timeout = 3000; // ms
	int timeoutCount = 3;

	const int eventNumber = 10;
	struct epoll_event events[eventNumber];
	while(true)
	{
		int ret = epoll_wait(epollFd, events, eventNumber, timeout);
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
			timeoutCount--;

			// for close case
			if (timeoutCount == 2)
			{
				printf("close pipeFds[1]...\n");
				close(pipeFds[1]);
				pipeFds[1] = -1;
			}

			break;
		default:
			for (int i = 0; i < ret; i++)
			{
				int e = events[i].events;
				int fd = events[i].data.fd;
				if (e & EPOLLIN)
				{
					char buffer[1024];
					ssize_t readSize = read(fd, buffer, 1023);
					buffer[readSize] = 0;
					printf("read [%s] from fd: %d, size is %zd\n", buffer, fd, readSize);
				}
				else if (e & (EPOLLHUP | EPOLLERR))
				{
					// if all fds are cloesd, should break
					printf("will close fd(%d), event is %d\n", fd, e);
					close(fd);
					if (fd == pipeFds[0])
						pipeFds[0] = -1;
				}
			}
		}

		if (timeoutCount <= 0)
			break;
	}

	if (pipeFds[0] > 0)
		close(pipeFds[0]);
	if (pipeFds[1] > 0)
		close(pipeFds[1]);

	return 0;
}