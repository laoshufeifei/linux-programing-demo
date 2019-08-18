#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>
#include <stdbool.h>
#include <string.h>


int main(void)
{
	int pipeFd[2];
	int ret = pipe(pipeFd);

	struct pollfd pollFd[1];
	pollFd[0].fd = pipeFd[0];
	pollFd[0].events = POLLIN;

	ret = write(pipeFd[1], "abcd", 4);

	int timeoutNum = 0;
	int timeout = 5000;
	while (true)
	{
		ret = poll(pollFd, 1, timeout);
		if (ret == -1)
		{
			perror("poll");
			return 1;
		}
		else if (ret == 0)
		{
			printf("timeout: %d ms\n", timeout);
			if (++timeoutNum > 2)
			{
				break;
			}
			continue;
		}
		else
		{
			if (pollFd[0].revents & POLLIN)
			{
				char ch[1];
				ret = read(pollFd[0].fd, ch, 1);
				printf("readable: [%c]\n", ch[0]);
			}
		}
	}

	close(pipeFd[0]);
	close(pipeFd[1]);
	return 0;
}
