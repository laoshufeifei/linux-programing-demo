#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <pthread.h>

static long counter = 0;
// static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t* mutex = NULL;

static void* _func(void* data)
{
    long maxTestNumber = (long)data;
    for (long i = 0; i < maxTestNumber; i++)
    {
        int ret = pthread_mutex_lock(mutex);
        if (ret != 0)
        {
            printf("lock mutex failed, ret is %d\n", ret);
            return NULL;
        }

        counter++;

        ret = pthread_mutex_unlock(mutex);
        if (ret != 0)
        {
            printf("unlock mutex failed, ret is %d\n", ret);
            return NULL;
        }
    }
    return NULL;
}


int main(void)
{
    int ret = 0;
    long maxTestNumber = 10000000;
    mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

    pthread_mutexattr_t attr;
    ret = pthread_mutexattr_init(&attr);
    if (ret != 0)
    {
        printf("mutex attr init failed, ret is %d\n", ret);
        return -1;
    }

    // PTHREAD_MUTEX_ERRORCHECK  PTHREAD_MUTEX_RECURSIVE
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (ret != 0)
    {
        printf("pthread_mutex_init failed, ret is %d\n", ret);
        return -1;
    }

    ret = pthread_mutex_init(mutex, &attr);
    if (ret != 0)
    {
        printf("pthread_mutex_init failed, ret is %d\n", ret);
        return -1;
    }

    ret = pthread_mutex_lock(mutex);
    if (ret == 0)
    {
        printf("lock mutex succeeded\n");
        ret = pthread_mutex_trylock(mutex);
        if (ret == 0)
        {
            printf("try lock mutex ret is %d\n", ret);
            pthread_mutex_unlock(mutex);
        }
        else
        {
            printf("try lock mutex failed, ret is %d\n", ret);
        }

        pthread_mutex_unlock(mutex);
    }
    else
    {
        printf("lock mutex failed, ret is %d\n", ret);
        return -1;
    }

    // ignore pthread_create return value
    pthread_t thread1;
    pthread_create(&thread1, NULL, _func, (void*)maxTestNumber);

    pthread_t thread2;
    pthread_create(&thread2, NULL, _func, (void*)maxTestNumber);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("counter is %ld\n", counter);

    pthread_mutex_destroy(mutex);
    free((void*)mutex);
    return 0;
}