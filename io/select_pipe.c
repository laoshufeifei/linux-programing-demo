#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>

#define int64 long long

int main()
{
	// int fd = open("test.txt", O_RDONLY, 0);
	// printf("open new file fd is %d, errno is %d\n", fd, errno);
	// close(fd);

	int pipeFd[2];
	pipe(pipeFd);
	write(pipeFd[0], "abcd", 4);

	// fds
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(pipeFd[1], &fds);

	// wait up to file seconds
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	int ret = select(1, &fds, NULL, NULL, &tv);
	switch (ret)
	{
	case -1:
		printf("select had error, return is %d, errno is %d\n", ret, errno);
		break;
	case 0:
		printf("no data within xxx seconds\n");
		break;
	default:
		printf("data is available now.\n");
		break;
	}

	return 0;
}