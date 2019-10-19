#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

// http://man7.org/linux/man-pages/man2/semctl.2.html
union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

static int _semCreateOrGet(bool *createNew)
{
	char* pathName = "/tmp";
	key_t key = ftok(pathName, 1);

	int initValue = 1;
	int semId = semget(key, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if (semId != -1)
	{
		*createNew = true;
		printf("create an new sem succeeded\n");

		union semun arg;
		arg.val = 0;
		if (semctl(semId, 0, SETVAL, arg) == -1)
		{
			printf("semctl failed, errno: %d\n", errno);
			return -1;
		}

		struct sembuf buf;
		buf.sem_num = 0;
		buf.sem_op = initValue;
		buf.sem_flg = 0;
		if (semop(semId, &buf, 1) == -1)
		{
			printf("semop failed, errno: %d\n", errno);
			return -1;
		}
	}
	else
	{
		*createNew = false;
		if (errno != EEXIST)
		{
			printf("semget failed, errno is %d\n", errno);
			return -1;
		}

		printf("open an existing sem\n");
		semId = semget(key, 1, S_IRUSR | S_IWUSR);
		if (semId == -1)
		{
			printf("semget failed, errno is %d\n", errno);
			return -1;
		}

		struct semid_ds ds;
		union semun arg;
		arg.buf = &ds;
		for (int j = 0; j < 3; j++)
		{
			if (semctl(semId, 0, IPC_STAT, arg) == -1)
			{
				printf("semctl failed, errno is %d\n", errno);
				return -1;
			}

			// must wait sem_otime != 0
			if (ds.sem_otime != 0)
			{
				break;
			}

			sleep(1);
		}

		if (ds.sem_otime == 0)
		{
			printf("existing semaphore not initialized\n");
			return -1;
		}
	}

	return semId;
}

static void _showSemValue(int semId)
{
	int ret = semctl(semId, 0, GETVAL);
	if (ret == -1)
	{
		printf("semctl failed, errno is %d\n", errno);
		return;
	}

	printf("sem value is %d\n", ret);
}

static int _semValue(int semId)
{
	int ret = semctl(semId, 0, GETVAL);
	if (ret == -1)
	{
		printf("semctl failed, errno is %d\n", errno);
	}
	return ret;
}

static bool _semWait(int semId)
{
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;
	if (semop(semId, &buf, 1) == -1)
	{
		printf("semop failed, errno: %d\n", errno);
		return false;
	}

	printf("after %s sem value is %d\n", __FUNCTION__, _semValue(semId));
	return true;
}

static bool _trySemWait(int semId)
{
	int ret = semctl(semId, 0, GETVAL);
	if (ret == -1)
	{
		printf("semctl failed, errno is %d\n", errno);
		return false;
	}

	printf("%s return %s, sem value is %d\n", __FUNCTION__, ret > 0 ? "true" : "false", _semValue(semId));
	return ret > 1;
}

static bool _semPost(int semId)
{
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO;
	if (semop(semId, &buf, 1) == -1)
	{
		printf("semop(+1) for post failed, errno: %d\n", errno);
		return false;
	}

	printf("after %s sem value is %d\n", __FUNCTION__, _semValue(semId));
	return true;
}

int main(void)
{
	bool createNew = true;
	int semId = _semCreateOrGet(&createNew);
	if (semId == -1)
	{
		printf("semId == -1, errno is %d\n", errno);
		return -1;
	}

	printf("sem value is %d\n", _semValue(semId));

	_semWait(semId);
	_semPost(semId);

	// if your watch test SEM_UNDO, just exit main function after call _semWait or _semPost
	// return -1;

	_semWait(semId);
	_trySemWait(semId);

	// semctl(semId, 0, IPC_RMID);	// delete the semaphore
	return 0;
}