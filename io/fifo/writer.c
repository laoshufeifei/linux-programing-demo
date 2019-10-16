#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define FIFO_NAME "/tmp/fifo_test"

int main(void)
{
	if (access(FIFO_NAME, F_OK) == -1)
	{
		int ret = mkfifo("/tmp/fifo_test", S_IRWXU | S_IRWXG | S_IROTH);
		if (ret == -1)
		{
			printf("mkfifo failed, errno is %d\n", errno);
			return -1;
		}
	}

	printf("before open %s\n", FIFO_NAME);
	int fd = open(FIFO_NAME, O_WRONLY);
	printf("after open %s\n", FIFO_NAME);

	if (fd == -1)
	{
		printf("open failed, errno is %d\n", errno);
		return -1;
	}

	char* buffer = "abcdefghx";
	write(fd, buffer, strlen(buffer));
}