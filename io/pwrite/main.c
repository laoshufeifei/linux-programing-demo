#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

// pread, pwrite - read from or write to a file descriptor at a given offset

int main(void)
{
	int fd = open("test.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
	if (fd < 0)
	{
		printf("fd is %d, errno is %d\n", fd, errno);
		return -1;
	}

	long offSet = (long )lseek(fd, 0, SEEK_CUR);
	printf("offSet is %ld\n", offSet);

	write(fd, "abcd", 4);
	offSet = (long)lseek(fd, 0, SEEK_CUR);
	printf("offSet is %ld after write  4 bytes\n", offSet);

	pwrite(fd, "abcd", 4, 4);
	offSet = (long)lseek(fd, 0, SEEK_CUR);
	printf("offSet is %ld after pwrite 4 bytes\n", offSet);

	if (fd > 0)
	{
		close(fd);
		remove("test.txt");
	}

	return 0;
}