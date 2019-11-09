#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <malloc.h>

extern char etext, edata, end;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    static long long i;
    static long long ii = 0;
    _showAddr();

    void* p = alloca(1024);
    printf("alloca(1024) return %p\n", p);

    _sbrkTest();

    void* pp = memalign(1024, 4096);
    printf("memalign       return is %p\n", pp);
    free(pp);

    posix_memalign(&pp, 1024, 4096);
    printf("posix_memalign return is %p\n", pp);
    free(pp);

    pp = malloc(0);
    printf("malloc(0) return %p\n", p);
    free(pp);

    pp = malloc(1);
    printf("malloc(1) return %p\n", p);
    free(pp);

    free(NULL);
    return 0;
}