#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <malloc.h>

extern char etext, edata, end;

static void _showAddr()
{
    printf("head end sbrk(0)          %10p\n", sbrk(0));
    printf("uninitialized data (end)  %10p\n", &end);
    printf("initialized data (edata)  %10p\n", &edata);
    printf("program text (etext)      %10p\n", &etext);
    printf("\n");
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    static long long i;
    static long long ii = 0;
    _showAddr();

    void* p = malloc(1);
    printf("malloc(1) return %p\n", p);
    _showAddr();

    void* p0 = malloc(0);
    printf("malloc(0) return %p\n", p0);
    _showAddr();

    free(p);
    free(p0);


    printf("alloca(1024) return %p\n", alloca(1024));
    printf("alloca(1024) return %p\n", alloca(1024));


    p = memalign(1, 4096);
    printf("memalign(1, 4096) return is %p\n", p);
    free(p);


    posix_memalign(&p, 1024, 4096);
    printf("posix_memalign(1024, 4096) return is %p\n", p);
    free(p);


    free(NULL);
    return 0;
}