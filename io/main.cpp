#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define int64 long long

	// 1 test big file
	// 2 test pipe file
	// 3 test select/poll/epoll

int main()
{
	printf("size of int is %zd\n", sizeof(int));
	printf("size of long is %zd\n", sizeof(long));
	printf("size of long is %zd\n", sizeof(long long));

	bool isIn64 = sizeof(void*) == 8;
	printf("is in 64 %d\n", isIn64);

	// create new file with 774 mode
	int fd = open("test.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
	printf("fd is %d, error no. %d\n", fd, errno);

	const int onePart = 1024;
	char buff[onePart];
	for (int i = 0; i < onePart; i++)
	{
		buff[i] = i % 10 + '0';
	}
	buff[onePart - 1] = '\n';

	int64 totalSize = 1024L * 1024L * 1024L * 5;
	for (long long i = 0; i < totalSize / onePart; i++)
	{
		ssize_t writeSize = write(fd, buff, onePart);
		if (writeSize != onePart)
		{
			printf("%lld write %zd, error no. %d\n", i, writeSize, errno);
		}

		int64 offSet = (int64)lseek(fd, 0, SEEK_CUR);
		if (offSet != (i + 1) * onePart)
		{
			printf("%lld lseek(%lld) had error error no. %d\n", i, offSet, errno);
		}
		// break;
	}

	close(fd);
}