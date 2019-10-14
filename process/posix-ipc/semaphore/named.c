#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

// gcc main.c -lpthread

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

int main()
{
	const char* semMame = "test_semaphore";

	bool createNew = true;
	unsigned int initValue = 0;
	sem_t* sem = sem_open(semMame, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, initValue);
	if (sem == SEM_FAILED)
	{
		createNew = false;
		sem = sem_open(semMame, 0);
		printf("open existing semaphore\n");
	}
	else
	{
		printf("create new semaphore, init value is %u\n", initValue);
	}

	if (sem == SEM_FAILED)
	{
		printf("sem_open failed, errno is %d\n", errno);
		return -1;
	}

	_printSemValue(sem);

	int ret = 0;
	ret = sem_trywait(sem);
	printf("sem_trywait %s(%d)\n", ret == 0 ? "success" : "failed", ret);

	printf("will call sem_post\n");
	sem_post(sem);
	_printSemValue(sem);

	ret = sem_trywait(sem);
	printf("sem_trywait %s(%d)\n", ret == 0 ? "success" : "failed", ret);
	_printSemValue(sem);

	sem_close(sem);
	if (createNew)
	{
		sem_unlink(semMame);
	}
	return 0;
}