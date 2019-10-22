#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

static int _childFunc(void* addr, size_t mapSize, int sleepTime)
{
	char* p = (char*)addr;
	printf("\tbefore modify is [%s]\n", p);
	for (size_t i = 0; i < mapSize; i++)
	{
		p[i] += 'A' - 'a';
	}

	printf("\tchange upper case by modify addr\n");
	sleep(sleepTime);

	// ignore check addr
	int ret = msync(addr, mapSize, MS_SYNC);
	if (ret == -1)
	{
		printf("msync failed, errno is %d\n", errno);
		return -1;
	}

	// ignore check addr
	ret = munmap(addr, mapSize);
	if (ret == -1)
	{
		printf("munmap failed, errno is %d\n", errno);
		return -1;
	}
}

// must call free
static char* _fileContext(char* fileName)
{
	int fd = open(fileName, O_RDONLY);
	if (fd == -1)
	{
		printf("open %s failed, errno is %d\n", fileName, errno);
		return NULL;
	}

	struct stat sb;
	fstat(fd, &sb);
	size_t fileSize = sb.st_size;
	char* buffer = (char*)malloc(fileSize + 1);

	size_t totalReadSize = 0;
	while (totalReadSize < fileSize)
	{
		ssize_t readSize = read(fd, buffer, fileSize - totalReadSize);
		if (readSize < 0)
		{
			printf("read failed, errno is %d\n", errno);
			break;
		}
		else if (readSize == 0)
		{
			if (totalReadSize != fileSize)
			{
				printf("read end of file, but totalReadSize(%zu) != fileSize(%zu)\n", totalReadSize, fileSize);
			}
			break;
		}
		totalReadSize += readSize;
	}
	buffer[totalReadSize] = 0;

	return buffer;
}

int main(int argc, char* argv[])
{
	if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
	{
		printf("as default will use shared mmap\n");
		printf("if argc > 1 will use private mmap\n");
		return 0;
	}

	bool usePrivate = argc > 1;
	printf("will use %s flag for mmap\n", usePrivate ? "private" : "shared");

	static long pageSize = 0;
	if (pageSize == 0)
	{
		pageSize = sysconf(_SC_PAGESIZE);
	}
	printf("page size is %ld\n", pageSize);

	char* fileName = "test.txt";
	int fd = open(fileName, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRWXG | S_IROTH);
	if (fd == -1)
	{
		printf("open %s failed, errno is %d\n", fileName, errno);
		return -1;
	}

	char* buffer = "abcdefgh";
	const size_t mapSize = strlen(buffer);
	write(fd, buffer, mapSize);

	int flag = usePrivate ? MAP_PRIVATE : MAP_SHARED;
	void* addr = mmap(NULL, mapSize, PROT_READ | PROT_WRITE, flag, fd, 0);
	close(fd);

	if (addr == MAP_FAILED)
	{
		printf("mmap failed, errno is %d\n", errno);
		return -1;
	}
	printf("mmap return %p\n", addr);

	pid_t pid = fork();
	switch (pid)
	{
	case -1:
		printf("fork error\n");
		return -1;
	case 0:
	{
		printf("\tchild: in\n");
		_childFunc(addr, mapSize, 2);
		char* buff = _fileContext(fileName);
		printf("\tchild: file context is [%s]\n", buff);
		free(buff);
		printf("\tchild: out\n");
		return 0;
		break;
	}
	default:
		{
			wait(NULL);
			char* buff = _fileContext(fileName);
			printf("parent: file context is [%s]\n", buff);
			free(buff);
			printf("parent: will exit\n");
		}
		break;
	}

	// system("od -c -w8 test.txt");
	// unlink(fileName);
	return 0;
}
