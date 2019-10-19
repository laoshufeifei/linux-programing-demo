#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>

// i++
#define atomicAddValue __sync_fetch_and_add
#define atomicSetValue __sync_lock_test_and_set

int main(void)
{
	// pathName must existing(file or directory)
	// if pathname and proj is same, key_t is same
	char* pathName = "/tmp";
	key_t key1 = ftok(pathName, 1);
	key_t key2 = ftok(pathName, 1);
	key_t key3 = ftok(pathName, 3);
	printf("key1 is %d\nkey2 is %d\nkey3 is %d\n", (int)key1, (int)key2, (int)key3);
	printf("\n");

	int shmSize = sizeof(int);
	bool createNew = true;
	// if use IPC_PRIVATE can't share memory with other process
	int shmId = shmget(key1, shmSize, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (shmId == -1)
	{
		createNew = false;
		if (errno == EEXIST)
		{
			shmId = shmget(key1, shmSize, S_IRUSR | S_IWUSR);
			printf("\treader: open an existing shm\n");
		}
		else
		{
			printf("shmget failed, return is %d, errno is %d\n", shmId, errno);
			return -1;
		}
	}
	else
	{
		printf("create an new shm succeeded\n");
	}

	if (shmId == -1)
	{
		printf("shmId is -1, errno is %d\n", errno);
		return -1;
	}

	void* addr = shmat(shmId, NULL, 0);
	if (addr == (void*)-1)
	{
		printf("shmat failed, errno is %d\n", errno);
		return -1;
	}
	int* number = (int*)addr;

	int maxTestCount = 200000000;
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

	printf("\n");
	int ret = shmdt(addr);
	printf("shmdt %s\n", ret == 0 ? "succeeded" : "failed");
	ret = shmctl(shmId, IPC_RMID, NULL);
	printf("shmctl %s\n", ret == 0 ? "succeeded" : "failed");
	return 0;
}
