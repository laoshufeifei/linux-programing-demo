#include <stdio.h>

// gcc main.c -o test && ./test > test.log 2>test.log && wc -l test.log
// gcc main.c -o test && ./test > test.log 2>&1       && wc -l test.log

int main(void)
{
	for (int i = 0; i < 20000; i++)
	{
		fprintf(stdout, "%d\n", i);
		fprintf(stderr, "\t%d\n", i);
	}

	return 0;
}
