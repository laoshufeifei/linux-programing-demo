#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>

#define int64 long long
#define max(X,Y) ((X) > (Y) ? (X) : (Y))

int main()
{
	// Disk files are always ready to read 
	// (but the read might return 0 bytes if you're already at the end of the file)
	// https://stackoverflow.com/questions/11901884/how-can-select-wait-on-regular-file-descriptors-non-sockets

	bool watchNormalFile = true;
	int normalFd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IROTH);
	printf("open new file fd is %d, errno is %d\n", normalFd, errno);
	write(normalFd, "1234", 4);
	lseek(normalFd, 0, SEEK_SET);

	int pipeFds[2] = {-1, -1};
	pipe(pipeFds);
	write(pipeFds[1], "abcd", 4);

	fd_set fds;
	int maxFd = 0;
	int timeoutCount = 5;

	while(true)
	{
		FD_ZERO(&fds);
		FD_SET(pipeFds[0], &fds);
		maxFd = max(maxFd, pipeFds[0]);

		FD_SET(STDIN_FILENO, &fds);
		maxFd = max(maxFd, STDIN_FILENO);

		if (watchNormalFile && normalFd > 0)
		{
			FD_SET(normalFd, &fds);
			maxFd = max(maxFd, normalFd);
		}

		// reset timeval everytime
		struct timeval tv;
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		int ret = select(maxFd + 1, &fds, NULL, NULL, &tv);
		// printf("after select tv_sec is %ld tv_usec is %ld\n", tv.tv_sec, tv.tv_usec);
		switch (ret)
		{
		case -1:
			printf("select had error, return is %d, errno is %d\n", ret, errno);
			break;
		case 0:
			printf("timeout...\n");
			timeoutCount--;
			break;
		default:
			// printf("data is available now\n");
			// notice i <= maxFd
			for (int i = 0; i <= maxFd; i++)
			{
				if (FD_ISSET(i, &fds))
				{
					char buffer[1024];
					ssize_t readSize = read(i, buffer, 1023);
					buffer[readSize] = 0;
					printf("read [%s] from fd: %d, size is %zd\n", buffer, i, readSize);
				}
			}
		}

		break;
		if (timeoutCount <= 0)
			break;
	}

	if (normalFd > 0)
	{
		close(normalFd);
		remove("test.txt");
	}

	if (pipeFds[0] > 0)
		close(pipeFds[0]);
	if (pipeFds[1] > 0)
		close(pipeFds[1]);
	return 0;
}