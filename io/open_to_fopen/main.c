#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

int main(void)
{
	int fd = open("test.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
	printf("fd is %d, errno is %d\n", fd, errno);

	FILE* file = fdopen(fd, "w");
	if (file == NULL)
	{
		printf("file is NULL\n");
		return 1;
	}
	else
	{
		int fd2 = fileno(file);
		printf("fd is %d, fd2 is %d\n", fd, fd2);
	}

	if (fd > 0)
	{
		close(fd);
		remove("test.txt");
	}
	return 0;
}
