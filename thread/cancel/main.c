#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <pthread.h>

void* _func(void* data)
{
    int oldType = 0;
    int ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldType);
    printf("\t set cancel type to DEFERED(%d), ret = %d, errno = %d, oldType = %d\n",
        (int)PTHREAD_CANCEL_DEFERRED, ret, errno, oldType);

    ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldType);
    printf("\t set cancel state to DISABLED(%d), ret = %d, errno = %d, oldType = %d\n",
        (int)PTHREAD_CANCEL_DISABLE, ret, errno, oldType);

    long t = (long)data;
    pthread_t tId = pthread_self();

    for (long i = 0; i < t; i++)
    {
        printf("\t in %p thread: will sleep 1 s\n", (void*)tId);
        sleep(1);
        if (i == 0)
        {
            ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldType);
            printf("\t set cancel state to DISABLED(%d), ret = %d, errno = %d, oldType = %d\n",
                (int)PTHREAD_CANCEL_ENABLE, ret, errno, oldType);
        }
    }

    printf("\t in %p thread: sleep finish, will return %ld\n", (void*)tId, t);
    return (void*)t;
}

int main(void)
{
    int ret = 0;
    pthread_t thread;
    ret = pthread_create(&thread, NULL, _func, (void*)3);
    if (ret != 0)
    {
        printf("pthread_cread failed, ret is %d\n", ret);
        return -1;
    }

    sleep(1);
    printf("will cancel thread %p\n", (void*)thread);
    ret = pthread_cancel(thread);
    if (ret != 0)
    {
        printf("pthread_cancel failed, ret is %d\n", ret);
        return -1;
    }

    void* threadRet;
    ret = pthread_join(thread, &threadRet);
    if (ret != 0)
    {
        printf("pthread_join failed, ret is %d\n", ret);
        return -1;
    }

    if (threadRet == PTHREAD_CANCELED)
    {
        printf("child was cancelled\n");
    }
    else
    {
        printf("child thread exit normally, return %ld\n", (long)threadRet);
    }

    return 0;
}