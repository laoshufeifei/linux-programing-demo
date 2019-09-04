#include <stdio.h>

int main(void)
{
	const char* cmd = "ls";

	// must be 'r' OR 'w'
	FILE* fp = popen(cmd, "r");
	if (fp == NULL)
	{
		printf("popen return NULL\n");
		return 1;
	}

	char buffer[512];
	while (fgets(buffer, 512, fp) != NULL)
	{
		printf("%s", buffer);
	}

	pclose(fp);
}
