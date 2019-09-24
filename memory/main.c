#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <malloc.h>

extern char etext, edata, end;

static void _showAddr()
{
    printf("end   is %p\n", &end);
    printf("edata is %p\n", &edata);
    printf("etext is %p\n", &etext);
}

static void _sbrkTest()
{
    const int testCount = 1000;
    void* addrs[testCount];

    printf("before alloc %d * 1024, sbrk return %10p\n", testCount, sbrk(0));
    for (int i = 0; i < testCount; i++)
    {
        void* mem = malloc(1024);
        addrs[i] = mem;
    }
    printf("after  alloc %d * 1024, sbrk return %10p\n", testCount, sbrk(0));

    for (int i = testCount - 1; i >= 0; i--)
    {
        free(addrs[i]);
    }
    printf("after  free  %d * 1024, sbrk return %10p\n", testCount, sbrk(0));
}

long long main(int argc, char* argv[])
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