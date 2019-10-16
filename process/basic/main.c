#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>

int main(void)
{
	long patendPid = (long)getpid();

	pid_t pid = fork();
	switch (pid)
	{
	case -1:
		printf("fork error, errno is %d\n", errno);
		return -1;
	case 0:
	{
		long childPid = (long)getpid();
		printf("\tchild(%ld): in \n", childPid);
		sleep(1);
		printf("\tchild(%ld): out\n", childPid);
		return 0;
	}
	default:
		printf("parent(%ld): wait for child(%ld) ........\n", patendPid, (long)pid);
		wait(NULL);
		printf("parent(%ld): wait for child(%ld) finished\n", patendPid, (long)pid);
		break;
	}

	return 0;
}