#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <pthread.h>

static pthread_key_t key;   // Thread-specific Data
static __thread long tls;   // Thread-local storage

static void* _func(void* data)
{
    void* tId = (void*)pthread_self;
    tls = (long)data;
    char* space = tls == 1 ? "" : "    "; // distinguish different thread for log
    printf("%s%p thread: set tls as %ld\n", space, tId, tls);

    void* buffer = malloc(16);
    int ret = pthread_setspecific(key, buffer);
    if (ret != 0)
    {
        printf("%s%p thread: set specific data failed, errno is %d\n", space, tId, errno);
        return 0;
    }
    printf("%s%p thread: set specific data %p\n", space, tId, buffer);

    sleep(tls);

    buffer = pthread_getspecific(key);
    if (buffer == NULL)
    {
        printf("%s%p thread: get specific data return NULL\n", space, tId);
    }
    else
    {
        printf("%s%p thread: get specific data return %p\n", space, tId, buffer);
    }

    printf("%s%p thread: tls is %ld\n", space, tId, tls);
    return 0;
}

static void _cleanUp(void* buffer)
{
    void* tId = (void*)pthread_self;
    printf("%p thread: in function %s, free %p\n", tId, __func__, buffer);
    free(buffer);
}

int main(void)
{
    int ret = 0;

    ret = pthread_key_create(&key, _cleanUp);
    if (ret != 0)
    {
        printf("pthread_key_create failed, ret is %d\n", ret);
        return -1;
    }

    pthread_t thread1;
    ret = pthread_create(&thread1, NULL, _func, (void*)1);
    if (ret != 0)
    {
        printf("pthread_cread failed, ret is %d\n", ret);
        return -1;
    }

    pthread_t thread2;
    ret = pthread_create(&thread2, NULL, _func, (void*)2);
    if (ret != 0)
    {
        printf("pthread_cread failed, ret is %d\n", ret);
        return -1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}