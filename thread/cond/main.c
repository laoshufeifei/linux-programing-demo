#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include <pthread.h>

static long counter = 0;
static bool producerExit = false;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void* _customerFunc(void* data)
{
    int ret = 0;
    long maxTestNumber = (long)data;

    for (;;)
    {
        pthread_mutex_lock(&mutex);
        while (counter == 0)
        {
            if (!producerExit)
            {
                printf("\t before wait\n");
                pthread_cond_wait(&cond, &mutex);
                printf("\t after  wait\n");
            }
            else
            {
                break;
            }
        }

        while (counter > 0)
        {
            counter--;
            printf("\t exhaust %ld\n", counter + 1);
        }

        pthread_mutex_unlock(&mutex);

        if (producerExit && counter == 0)
            break;

        sleep(3);
    }

    printf("consumer thread finished\n");
    return NULL;
}

static void* _productFunc(void* data)
{
    int ret = 0;
    long maxTestNumber = (long)data;

    for (long i = 0; i < maxTestNumber; i++)
    {
        pthread_mutex_lock(&mutex);
        printf("production %ld\n", counter + 1);
        counter++;
        pthread_mutex_unlock(&mutex);

        ret = pthread_cond_signal(&cond);
        if (ret != 0)
        {
            printf("cond_signal failed, ret is %d\n", ret);
            return NULL;
        }

        sleep(2);
    }

    printf("producer thread finished\n");
    producerExit = true;
    return NULL;
}


int main(void)
{
    int ret = 0;
    long maxTestNumber = 10;

    // ignore pthread_create return value
    pthread_t thread1;
    pthread_create(&thread1, NULL, _productFunc, (void*)maxTestNumber);

    pthread_t thread2;
    pthread_create(&thread2, NULL, _customerFunc, (void*)maxTestNumber);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("counter is %ld\n", counter);

    return 0;
}