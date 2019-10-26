#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <pthread.h>

// http://man7.org/linux/man-pages/man3/pthread_spin_lock.3.html
static long counter = 0;
static pthread_spinlock_t lock;

static void* _func(void* data)
{
    long maxTestNumber = (long)data;
    for (long i = 0; i < maxTestNumber; i++)
    {
        int ret = pthread_spin_lock(&lock);
        if (ret != 0)
        {
            printf("spin lock failed, ret is %d\n", ret);
            return NULL;
        }

        counter++;

        ret = pthread_spin_unlock(&lock);
        if (ret != 0)
        {
            printf("spin unlock failed, ret is %d\n", ret);
            return NULL;
        }
    }
    return NULL;
}


int main(void)
{
    int ret = 0;
    long maxTestNumber = 100000000;

    ret = pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
    if (ret != 0)
    {
        printf("spin lock failed, ret is %d\n", ret);
        return -1;
    }

    // ignore pthread_create return value
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, _func, (void*)maxTestNumber);
    pthread_create(&thread2, NULL, _func, (void*)maxTestNumber);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("counter is %ld\n", counter);

    pthread_spin_destroy(&lock);
    return 0;
}