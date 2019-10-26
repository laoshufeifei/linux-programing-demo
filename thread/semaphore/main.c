// https://linux.die.net/man/3/pthread_mutexattr_init

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include "sem.h"

int main(void)
{
    char* semName = "/tmp/semaphore";
    bool createNew = true;

    semaphore_t* sem = semaphore_create(semName);
    if (sem == NULL)
    {
        createNew = false;
        sem = semaphore_open(semName);
        if (sem == NULL)
        {
            printf("    open semaphore failed, errno is %d\n", errno);
            return -1;
        }
        printf("    open semaphore succeeded\n");
    }
    else
    {
        printf("create semaphore succeeded\n");
    }

    char* space = createNew ? "" : "    ";
    printf("%swaitting...\n", space);
    semaphore_wait(sem);
    printf("%swait succeeded\n", space);

    sleep(3);

    semaphore_post(sem);
    printf("%spost semaphore\n", space);

    semaphore_close(sem);
    if (createNew)
    {
        unlink(semName);
    }
    return 0;
}
