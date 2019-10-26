#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include <pthread.h>

static long counter = 0;
static bool producerExit = false;
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

static void* _readFunc(void* data)
{
    int ret = 0;
    long maxTestNumber = (long)data;

    for (;;)
    {
        pthread_rwlock_rdlock(&rwlock);
        pthread_rwlock_unlock(&rwlock);

        if (counter == maxTestNumber * 2)
            break;

        // sleep(2);
    }

    printf("reader thread finished\n");
    return NULL;
}

static void* _writerFunc(void* data)
{
    int ret = 0;
    long maxTestNumber = (long)data;

    for (long i = 0; i < maxTestNumber; i++)
    {
        pthread_rwlock_wrlock(&rwlock);
        counter++;
        pthread_rwlock_unlock(&rwlock);

        // sleep(3);
    }

    printf("writer thread finished\n");
    producerExit = true;
    return NULL;
}


int main(void)
{
    int ret = 0;
    long maxTestNumber = 100000;

    // ignore pthread_create return value
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, _writerFunc, (void*)maxTestNumber);
    pthread_create(&thread2, NULL, _readFunc, (void*)maxTestNumber);

    sleep(1);
    pthread_t thread3, thread4;
    pthread_create(&thread3, NULL, _writerFunc, (void*)maxTestNumber);
    pthread_create(&thread4, NULL, _readFunc, (void*)maxTestNumber);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    printf("counter is %ld\n", counter);

    return 0;
}