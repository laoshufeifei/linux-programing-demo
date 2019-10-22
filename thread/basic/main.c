#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <pthread.h>

void* _func(void* data)
{
    long t = (long)data;
    printf("\t in child thread: will sleep %ld s\n", t);
    sleep(t);
    return (void*)t;
}

int main(void)
{
    pthread_t thread;
    int ret = pthread_create(&thread, NULL, _func, (void*)3);
    if (ret != 0)
    {
        printf("pthread_cread failed, errno is %d\n", errno);
        return -1;
    }
    printf("create a thread\n");

    void* retValue;
    ret = pthread_join(thread, &retValue);
    if (ret != 0)
    {
        printf("pthread_join failed, errno is %d\n", errno);
        return -1;
    }
    printf("child thread return %ld\n", (long)retValue);

    return 0;
}