#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void signalHandler(int signal)
{
	printf("received signal %d\n", signal);
}

int main(void)
{
	signal(SIGINT, SIG_IGN);
	sleep(5);
	printf("after sleep 1\n");

	signal(SIGINT, signalHandler);
	sleep(5);
	printf("after sleep 2\n");
}