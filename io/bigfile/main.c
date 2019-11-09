#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

// gcc -D_FILE_OFFSET_BITS=64 main.c

#define int64 long long
#define min(x,y) (((x)<(y))?(x):(y))

int main()
{
	printf("size of int is %zd\n", sizeof(int));
	printf("size of long is %zd\n", sizeof(long));
	printf("size of long is %zd\n", sizeof(long long));

	int64 _2G = (int64)1 << 31;
	int64 _4G = (int64)1 << 32;
	printf("2G is %lld 0x%llx\n", _2G, _2G);
	printf("4G is %lld 0x%llx\n", _4G, _4G);
	printf("max size_t is 0x%zx\n", (size_t)-1);

	bool isIn64 = sizeof(void*) == 8;
	printf("is in 64 %d\n", isIn64);

	// create new file with 774 mode
	int fd = open("test.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
	printf("fd is %d, errno is %d\n", fd, errno);

	int64 totalSize = (int64)1024 * 1024 * 1024 * 2 + 100;
	const size_t bufferSize = 4096;
	if (bufferSize > 0x7ffff000)
	{
		// http://man7.org/linux/man-pages/man2/write.2.html
		printf("write will transfer at most 0x7ffff000 (2,147,479,552) bytes\n");
		return 1;
	}

	char* buffer = (char*)malloc(bufferSize);
	memset(buffer, '1', bufferSize);

	printf("totalSize  is %lld\n", totalSize);
	printf("bufferSize is %zu\n", bufferSize);

	int64 i = 0;  // write time for log
	int64 hadWrittenSize = 0;
	bool hadError = false;
	while (hadWrittenSize < totalSize)
	{
		i++;

		// write() will transfer at most 0x7ffff000 (2,147,479,552) bytes
		int64 needsWriteSize = min(totalSize - hadWrittenSize, bufferSize);
		ssize_t wroteSize = write(fd, buffer, needsWriteSize);
		if (wroteSize != needsWriteSize)
		{
			printf("warning: %lld times: write %lld != %zu, errno is %d\n",
				i, (int64)wroteSize, bufferSize, errno);
		}
		hadWrittenSize += wroteSize;

		int64 offSet = (int64)lseek(fd, 0, SEEK_CUR);
		if (offSet != hadWrittenSize)
		{
			hadError = true;
			printf("error: %lld times: lseek had error(%lld != %lld), errno is %d\n",
				i, offSet, hadWrittenSize, errno);
		}

		// 27: EFBIG           File too large
		if (hadError && errno == 27)
		{
			printf("please use _FILE_OFFSET_BITS=64 for support big file\n");
			break;
		}
	}
	close(fd);
	printf("after write test, hadError? %d\n", hadError);
	free(buffer);

	// reopen
	fd = open("test.txt", O_RDWR);
	int64 offSet = (int64)lseek(fd, totalSize, SEEK_SET);
	printf("offSet is %lld\n", offSet);
	offSet = (int64)lseek(fd, 10, SEEK_SET);
	printf("offSet is %lld\n", offSet);

	if (fd > 0)
	{
		close(fd);
		remove("test.txt");
	}

	return 0;
}
