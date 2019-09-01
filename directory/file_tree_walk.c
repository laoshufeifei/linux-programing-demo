#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <ftw.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// http://www.man7.org/linux/man-pages/man3/nftw.3.html
static int walkInfo(const char* pathName, const struct stat* sb, int typeFlag, struct FTW* ftwBuff)
{
	switch (sb->st_mode & S_IFMT)
	{
	case S_IFREG:  printf("- "); break;
	case S_IFDIR:  printf("d "); break;
	case S_IFCHR:  printf("c "); break;
	case S_IFBLK:  printf("b "); break;
	case S_IFLNK:  printf("l "); break;
	case S_IFIFO:  printf("p "); break;
	case S_IFSOCK: printf("s "); break;
	default:       printf("? "); break;
	}

	switch (typeFlag)
	{
	case FTW_D: 	printf("D\t"); break;
	case FTW_DNR: 	printf("DNR\t"); break;
	case FTW_DP: 	printf("DP\t"); break;
	case FTW_F: 	printf("F\t"); break;
	case FTW_SL: 	printf("SL\t"); break;
	case FTW_SLN: 	printf("SLN\t"); break;
	case FTW_NS: 	printf("NS\t"); break;
	default:       	printf("?\t"); break;
	}

	if (typeFlag == FTW_NS)
		printf("-------");
	else
		printf("%7jd", (intmax_t)sb->st_size);

	const char* basePath = pathName + ftwBuff->base;
	printf("   %s(%s)\n", pathName, basePath);

#if defined(_GNU_SOURCE)
	if (strcmp(basePath, ".git") == 0)
	{
		printf("skip .git...............................\n");
		return FTW_SKIP_SUBTREE;
	}
#endif

	return 0;
}

static void fileTreeWalk(char* dirPath)
{
	// If FTW_DEPTH set, do a post-order traversal
	// If FTW_PHYS set, do not follow symbolic links.
	int flag = 0;
#if defined(_GNU_SOURCE)
	flag |= FTW_ACTIONRETVAL;
#endif

	int openFdNumber = 10;
	// openFdNumber specifies the maximum number of directories that nftw() will hold open simultaneously.
	int ret = nftw(dirPath, walkInfo, openFdNumber, flag);
	printf("nftw return %d\n", ret);
}

int main(int argc, char** argv)
{
	fileTreeWalk("..");
	return 0;
}
