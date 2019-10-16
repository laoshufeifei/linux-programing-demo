#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>

// gcc main.c -lpthread -lrt

static void _printSemValue(sem_t* sem)
{
	int value = -1;
	int ret = sem_getvalue(sem, &value);
	if (ret == -1)
	{
		printf("sem_getvalue failed, errno is %d\n", errno);
		return;
	}
	printf("sem value is %d\n", value);
}

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
	else
	{
		printf("creater: create a new shared memory\n");
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

	return mapAddress;
}

int main()
{
	int maxTestCount = 100000000;
	const char* mapMame = "unnamed_sem_on_shm";

	// shared memory
	int fd = 0;
	bool createNew = false;
	int mapSize = sizeof(sem_t) + sizeof(int);
	char* mapAddress = (char*)_mapSharedMemory(mapMame, mapSize, &fd, &createNew);
	if (mapAddress == NULL)
	{
		return -1;
	}

	sem_t* sem = (sem_t*)(mapAddress);
	int* number = (int*)(mapAddress + sizeof(sem_t));

	if (createNew)
	{
		int pshared = 1; // 0 is for thread; other is for process;
		int ret = sem_init(sem, pshared, 1);
		if (ret == -1)
		{
			printf("sem_init failed, errno is %d\n", errno);
			return -1;
		}
		else
		{
			printf("creater: sem_init as 1 succeeded\n");
			_printSemValue(sem);
		}
	}

	if (createNew)
	{
		printf("creater: begin test...\n");
		for (int i = 0; i < maxTestCount; i++)
		{
			sem_wait(sem);
			(*number)++;
			sem_post(sem);
		}
		printf("creater: finish test, number is %d\n", *number);
		// sem_destroy(sem);
	}
	else
	{
		printf("\treader: begin test...\n");
		for (int i = 0; i < maxTestCount; i++)
		{
			sem_wait(sem);
			(*number)++;
			sem_post(sem);
		}
		printf("\treader: finish test, number is %d\n", *number);
	}

	munmap(mapAddress, mapSize);
	close(fd);
	if (createNew)
		shm_unlink(mapMame);

	return 0;
}
