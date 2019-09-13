#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <limits.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/types.h>

// https://linux.die.net/man/3/dirname
// There are two different versions of basename() - the POSIX version described above,
// and the GNU version
// In the glibc implementation of the POSIX versions of these functions they modify their argument,
// and segfault when called with a static string like "/usr/".
// There is no GNU version of dirname().
// #include <string.h>
// #define _GNU_SOURCE
static void pathTest(char* binPath)
{
	printf("$0 is %s\n", binPath);

	char* dirName = dirname(binPath);
	printf("dirname is %s\n", dirName);
	// notice: binPath had been change
	printf("binPath is %s\n", binPath);

	char* baseName = basename(binPath);
	printf("basename is %s\n", baseName);
	// notice: binPath had been change
	printf("binPath is %s\n", binPath);

	char absPath[1024];
	realpath(binPath, absPath);
	printf("realpath is %s\n", absPath);
}

// http://man7.org/linux/man-pages/man3/readdir_r.3.html
// since version 2.24, glibc deprecates readdir_r()
static void dirRead(const char* dirPath)
{
	DIR* dir = opendir(dirPath);

	while (true)
	{
		struct dirent* entry = readdir(dir);
		if (entry == NULL)
			break;

		if (entry->d_type == DT_DIR)
		{
			printf("[%s] is dir\n", entry->d_name);
		}
		else if (entry->d_type == DT_REG)
		{
			printf("[%s] is normal type\n", entry->d_name);
		}
		else
		{
			printf("[%s] 's type is %d\n", entry->d_name, entry->d_type);
		}
	}

	closedir(dir);
}

int main(int argc, char** argv)
{
	char buffer[512];
	char* ret = getcwd(buffer, 512);
	printf("getcwd return is %s, buffer is %s\n", ret, buffer);

	pathTest(argv[0]);

	printf("\n\n");
	dirRead("..");

	return 0;
}
