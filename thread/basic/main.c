#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <pthread.h>

static void* _func(void* data)
{
    long t = (long)data;
    pthread_t tId = pthread_self();
    printf("\t in %p thread: will sleep %ld s\n", (void*)tId, t);
    sleep(t);
    printf("\t in %p thread: sleep finish, will return %ld\n", (void*)tId, t);
    return (void*)t;
}

static void _init(void)
{
    printf("at _init function, before sleep 1\n");
    sleep(1);
    printf("at _init function, after  sleep 1\n");
}

pthread_once_t once = PTHREAD_ONCE_INIT;

int main(void)
{
    int ret = 0;
    ret = pthread_once(&once, _init);
    if (ret != 0)
    {
        printf("pthread_init failed, ret is %d\n", ret);
        return -1;
    }

    pthread_t thread1;
    ret = pthread_create(&thread1, NULL, _func, (void*)3);
    if (ret != 0)
    {
        printf("pthread_cread failed, ret is %d\n", ret);
        return -1;
    }

    pthread_t thread2;
    ret = pthread_create(&thread2, NULL, _func, (void*)5);
    if (ret != 0)
    {
        printf("pthread_cread failed, ret is %d\n", ret);
        return -1;
    }

    // if main thread exit, thread2 will exit also
    ret = pthread_detach(thread2);
    if (ret != 0)
    {
        printf("pthread_det failed, ret is %d\n", ret);
        return -1;
    }

    void* threadRet;
    ret = pthread_join(thread1, &threadRet);
    if (ret != 0)
    {
        printf("pthread_join failed, ret is %d\n", ret);
        return -1;
    }
    printf("child thread return %ld\n", (long)threadRet);

    return 0;
}