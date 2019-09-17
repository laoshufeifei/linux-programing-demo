#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

typedef void (*sighandler_t)(int);

static void signalHandler(int signal)
{
	printf("received signal %d\n", signal);
}

int main(void)
{
	sighandler_t oldHandler = signal(SIGINT, SIG_IGN);
	if (oldHandler == SIG_ERR)
	{
		printf("signal return error, errno is %d\n", errno);
		return -1;
	}
	sleep(5);
	printf("after sleep 1\n");

	oldHandler = signal(SIGINT, signalHandler);
	if (oldHandler == SIG_ERR)
	{
		printf("signal return error, errno is %d\n", errno);
		return -1;
	}

	sleep(5);
	printf("after sleep 2\n");

	struct sigaction newAction;
	struct sigaction oldAction;
	newAction.sa_handler = signalHandler;
	newAction.sa_flags = 0;
	sigemptyset(&newAction.sa_mask);

	int ret = sigaction(SIGINT, &newAction, &oldAction);
	if (ret == -1)
	{
		printf("sigaction return -1, errno is %d\n", errno);
		return -1;
	}

	printf("oldAction: flag is %d\n", oldAction.sa_flags);

	sleep(5);
	printf("after sleep 3\n");
}