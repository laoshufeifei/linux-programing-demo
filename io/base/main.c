#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

int main(void)
{
	int fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IROTH);
	printf("open new file fd is %d, errno is %d\n", fd, errno);
	if (fd == -1)
		return -1;

	ssize_t writeSize = write(fd, "abcd", 4);
	printf("write size is %zd\n", writeSize);
	lseek(fd, 0, SEEK_SET);

	// intentional
	// read size is 1, read char is a
	// read size is 1, read char is b
	// read size is 1, read char is c
	// read size is 1, read char is d
	// read size is 0, read char is 
	// read size is 0, read char is 
	// ...
	char buffer;
	for (ssize_t i = 0; i < writeSize + 3; i++)
	{
		buffer = 0;
		ssize_t readSize = read(fd, &buffer, 1);
		printf("read size is %zd, read char is %c\n", readSize, buffer);
	}

	close(fd);
	unlink("test.txt");
	return 0;
}