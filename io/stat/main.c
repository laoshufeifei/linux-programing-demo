#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define int64 long long

int main(int argc, char *argv[])
{
	if (argc == 1
		|| (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)))
	{
		printf("usage: %s [-l] file\n"
			"	default will use stat()\n"
			"	add -l will use lstat()\n",
			argv[0]);
		return -1;
	}

	const char* fileName = "";
	bool useLStat = false;
	if (argc > 2 && strcmp(argv[1], "-l") == 0)
	{
		useLStat = true;
		fileName = argv[2];
		printf("will use lstat file: %s\n", fileName);
	}
	else
	{
		fileName = argv[1];
		printf("will use stat file: %s\n", fileName);
	}

	int ret = 0;
	struct stat sb;
	if (useLStat)
	{
		ret = lstat(fileName, &sb);
	}
	else
	{
		ret = stat(fileName, &sb);
	}

	if (ret == -1)
	{
		printf("stat or lstat failed, errno: %d\n", errno);
		return -1;
	}

	switch (sb.st_mode & S_IFMT)
	{
	case S_IFBLK: printf("block device\n"); 	break;
	case S_IFCHR: printf("character device\n"); break;
	case S_IFDIR: printf("directory\n"); 		break;
	case S_IFIFO: printf("FIFO/pipe\n"); 		break;
	case S_IFLNK: printf("symlink\n"); 			break;
	case S_IFREG: printf("regular file\n"); 	break;
	case S_IFSOCK: printf("socket\n"); 			break;
	default: printf("unknown?\n"); 				break;
	}

	printf("I-node number:            %ld\n", (long)sb.st_ino);
	printf("Mode:                     %lo (octal)\n", (unsigned long)sb.st_mode);
	printf("Link count:               %ld\n", (long)sb.st_nlink);

	printf("Preferred I/O block size: %ld bytes\n", (long)sb.st_blksize);
	printf("File size:                %lld bytes\n", (long long)sb.st_size);
	printf("Blocks allocated:         %lld\n", (long long)sb.st_blocks);

	printf("Last status change:       %s", ctime(&sb.st_ctime));
	printf("Last file access:         %s", ctime(&sb.st_atime));
	printf("Last file modification:   %s", ctime(&sb.st_mtime));

	return 0;
}