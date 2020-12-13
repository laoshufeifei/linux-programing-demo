#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	const char *fifo = "event.fifo";
	unlink(fifo);
	if (mkfifo(fifo, 0600) == -1)
	{
		perror("mkfifo return -1");
		return 1;
	}

	int fd = open(fifo, O_RDWR | O_NONBLOCK, 0);

	// Since Linux 2.6.8, the size argument is ignored, but must be greater than zero;
	int epollFd = epoll_create(1);
	if (epollFd < 0)
	{
		perror("epoll create return < 0");
		return 1;
	}

	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
		perror("epoll ctl return -1");

	int timeout = 30000; // ms
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
			break;
		default:
			printf("epoll ret is %d\n", ret);
			for (int i = 0; i < ret; i++)
			{
				int e = events[i].events;
				int fd = events[i].data.fd;
				if (e & EPOLLIN)
				{
					char buffer[1024];
					ssize_t readSize = read(fd, buffer, 1);
					buffer[readSize] = 0;
					printf("read [%s] from fd: %d, size is %zd\n", buffer, fd, readSize);
				}
				else if (e & (EPOLLHUP | EPOLLERR))
				{
					// if all fds are cloesd, should break
					printf("will close fd(%d), event is %d\n", fd, e);
					close(fd);
				}
			}
		}
	}

	unlink(fifo);
	return 0;
}