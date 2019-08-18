#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define int64 long long

int main()
{
	printf("size of int is %zd\n", sizeof(int));
	printf("size of long is %zd\n", sizeof(long));
	printf("size of long is %zd\n", sizeof(long long));

	bool isIn64 = sizeof(void*) == 8;
	printf("is in 64 %d\n", isIn64);

	// create new file with 774 mode
	int fd = open("test.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
	printf("fd is %d, errno is %d\n", fd, errno);

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
			printf("%lld write %zd, errno is %d\n", i, writeSize, errno);
		}

		int64 offSet = (int64)lseek(fd, 0, SEEK_CUR);
		if (offSet != (i + 1) * onePart)
		{
			printf("%lld lseek(%lld) had error, errno is %d\n", i, offSet, errno);
		}
		// break;
	}

	int64 offSet =  (int64)lseek(fd, 10, SEEK_END);
	printf("offSet is %lld\n", offSet);

	close(fd);
	return 0;
}