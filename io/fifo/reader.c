#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

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
	int fd = open(FIFO_NAME, O_RDONLY);
	printf("after open %s\n", FIFO_NAME);

	if (fd == -1)
	{
		printf("open failed, errno is %d\n", errno);
		return -1;
	}

	char ch;
	while (true)
	{
		size_t readSize = read(fd, &ch, 1);
		if (readSize != 1)
		{
			printf("error: read size is %zu\n", readSize);
			break;
		}

		printf("read: %c\n", ch);

		if (ch == 'x')
			break;
	}

}