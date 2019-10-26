// https://linux.die.net/man/3/pthread_mutexattr_init

struct semaphore
{
	pthread_mutex_t lock;
	pthread_cond_t nonzero;
	unsigned count;
};
typedef struct semaphore semaphore_t;

// init count is 1
semaphore_t* semaphore_create(char* semaphore_name);
semaphore_t* semaphore_open(char* semaphore_name);
void semaphore_close(semaphore_t* semap);

void semaphore_post(semaphore_t* semap);
void semaphore_wait(semaphore_t* semap);
