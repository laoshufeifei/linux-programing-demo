#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

// gcc main.c -lrt

// i++
#define atomicAddValue __sync_fetch_and_add
#define atomicSetValue __sync_lock_test_and_set

static void* _mapSharedMemory(const char* name, int size, int* openFd, bool* isCreate)
{
	bool createNew = true;
	int fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
	if (fd == -1)
	{
		createNew = false;
		fd = shm_open(name, O_RDWR, 0666);
		printf("\treader: open an exist shared memory\n");
	}
	*isCreate = createNew;

	if (fd == -1)
	{
		printf("shm_open had error, error = %d\n", errno);
		return NULL;
	}
	*openFd = fd;

	if (createNew)
	{
		ftruncate(fd, size);
	}

	void* mapAddress = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mapAddress == MAP_FAILED)
	{
		printf("mmap failed for refer count address, errno is %d\n", errno);
		return NULL;
	}

	if (createNew)
	{
		atomicSetValue((int*)mapAddress, 0);
		printf("creater: create a new shard memory succeeded, set count as 0\n");
	}

	return mapAddress;
}

int main(int argc, char *argv[])
{
	int maxTestCount = 200000000;
	const char* mapMame = "test_shared_memory";

	// shared memory
	int fd = 0;
	bool createNew = false;
	int mapSize = sizeof(int);
	void* mapAddress = _mapSharedMemory(mapMame, mapSize, &fd, &createNew);
	if (mapAddress == NULL)
	{
		return -1;
	}
	int* number = (int*)mapAddress;

	if (createNew)
	{
		printf("creater: begin test...\n");
		for (int i = 0; i < maxTestCount; i++)
		{
			atomicAddValue(number, 1);
		}
		printf("creater: finish test, number is %d\n", *number);
	}
	else
	{
		printf("\treader: begin test...\n");
		for (int i = 0; i < maxTestCount; i++)
		{
			atomicAddValue(number, 1);
		}
		printf("\treader: finish test, number is %d\n", *number);
	}

	munmap(mapAddress, mapSize);
	close(fd);
	if (createNew)
		shm_unlink(mapMame);

	return 0;
}
