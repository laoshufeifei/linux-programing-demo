#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int pipeFds[2] = {-1, -1};
	if (pipe(pipeFds) == -1)
	{
		printf("pipe return -1\n");
		return -1;
	}

	int maxWriteCount = 65536 + 1;
	for (int i = 0; i < maxWriteCount; i++)
	{
		ssize_t writeSize = write(pipeFds[1], "a", 1);
		if (writeSize != 1)
		{
			printf("%d write pipe return %ld\n", i, (long)writeSize);
		}
	}

	if (pipeFds[0] > 0)
		close(pipeFds[0]);
	if (pipeFds[1] > 0)
		close(pipeFds[1]);
}