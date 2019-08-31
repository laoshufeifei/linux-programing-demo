#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

int main(void)
{
	int pipeFds[2] = {-1, -1};
	pipe(pipeFds);
	write(pipeFds[1], "abcd", 4);

	const int pollFdNum = 2;
	struct pollfd pollFd[pollFdNum];
	pollFd[0].fd = pipeFds[0];
	pollFd[0].events = POLLIN;

	pollFd[1].fd = STDIN_FILENO;
	pollFd[1].events = POLLIN;

	int timeout = 5000;
	int timeoutCount = 5;

	while (true)
	{
		int ret = poll(pollFd, pollFdNum, timeout);
		switch (ret)
		{
		case -1:
			printf("poll had error, return is %d, errno is %d\n", ret, errno);
			break;
		case 0:
			printf("timeout...\n");
			timeoutCount--;
			break;
		default:
			// printf("poll ret is %d\n", ret);
			for (int i = 0; i < pollFdNum; i++)
			{
				if (pollFd[i].revents & POLLIN)
				{
					int fd = pollFd[i].fd;
					char buffer[1024];
					ssize_t readSize = read(fd, buffer, 1023);
					buffer[readSize] = 0;
					printf("read [%s] from fd: %d, size is %zd\n", buffer, fd, readSize);
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
