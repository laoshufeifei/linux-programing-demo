// https://linux.die.net/man/3/pthread_mutexattr_init

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include "sem.h"

semaphore_t* semaphore_create(char* semName)
{
	int fd = open(semName, O_RDWR | O_CREAT | O_EXCL, 0666);
	if (fd < 0)
		return NULL;
	ftruncate(fd, sizeof(semaphore_t));

	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);

	pthread_condattr_t condAttr;
	pthread_condattr_init(&condAttr);
	pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

	semaphore_t* sem = (semaphore_t*)mmap(NULL, sizeof(semaphore_t),
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	sem->count = 1;
	pthread_mutex_init(&sem->lock, &mutexAttr);
	pthread_cond_init(&sem->nonzero, &condAttr);

	close(fd);
	return sem;
}

semaphore_t* semaphore_open(char* semName)
{
	int fd = open(semName, O_RDWR, 0666);
	if (fd < 0)
		return NULL;

	semaphore_t* semap = (semaphore_t* )mmap(NULL, sizeof(semaphore_t),
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	close(fd);
	return semap;
}

void semaphore_post(semaphore_t* sem)
{
	pthread_mutex_lock(&sem->lock);
	if (sem->count == 0)
		pthread_cond_signal(&sem->nonzero);
	sem->count++;
	pthread_mutex_unlock(&sem->lock);
}

void semaphore_wait(semaphore_t* sem)
{
	pthread_mutex_lock(&sem->lock);
	while (sem->count == 0)
		pthread_cond_wait(&sem->nonzero, &sem->lock);
	sem->count--;
	pthread_mutex_unlock(&sem->lock);
}

void semaphore_close(semaphore_t* sem)
{
	munmap((void*)sem, sizeof(semaphore_t));
}