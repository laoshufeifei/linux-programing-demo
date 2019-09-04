#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/inotify.h>
#include <limits.h>

#define BUFFER_LENGTH (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static void displayEvent(struct inotify_event* e)
{
	printf("    wd =%2d; ", e->wd);
	if (e->cookie > 0)
		printf("cookie =%4d; ", e->cookie);

	printf("mask = ");
	if (e->mask & IN_ACCESS)        printf("%20s", "IN_ACCESS");
	if (e->mask & IN_ATTRIB)        printf("%20s", "IN_ATTRIB");
	if (e->mask & IN_CLOSE_NOWRITE) printf("%20s", "IN_CLOSE_NOWRITE");
	if (e->mask & IN_CLOSE_WRITE)   printf("%20s", "IN_CLOSE_WRITE");
	if (e->mask & IN_CREATE)        printf("%20s", "IN_CREATE");
	if (e->mask & IN_DELETE)        printf("%20s", "IN_DELETE");
	if (e->mask & IN_DELETE_SELF)   printf("%20s", "IN_DELETE_SELF");
	if (e->mask & IN_IGNORED)       printf("%20s", "IN_IGNORED");
	if (e->mask & IN_ISDIR)         printf("%20s", "IN_ISDIR");
	if (e->mask & IN_MODIFY)        printf("%20s", "IN_MODIFY");
	if (e->mask & IN_MOVE_SELF)     printf("%20s", "IN_MOVE_SELF");
	if (e->mask & IN_MOVED_FROM)    printf("%20s", "IN_MOVED_FROM");
	if (e->mask & IN_MOVED_TO)      printf("%20s", "IN_MOVED_TO");
	if (e->mask & IN_OPEN)          printf("%20s", "IN_OPEN");
	if (e->mask & IN_Q_OVERFLOW)    printf("%20s", "IN_Q_OVERFLOW");
	if (e->mask & IN_UNMOUNT)       printf("%20s", "IN_UNMOUNT");

	if (e->len > 0)
		printf(" name = %s", e->name);

	printf("\n");
}

int main(int argc, char* argv[])
{
	if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
	{
		printf("usage: %s pathName1 pathName2...\n", argv[0]);
		return 0;
	}

	int inotifyFd = inotify_init();
	if (inotifyFd == -1)
	{
		printf("inotify_init return -1\n");
		return -1;
	}

	for (int i = 1; i < argc; i++)
	{
		int wd = inotify_add_watch(inotifyFd, argv[i], IN_ALL_EVENTS);
		if (wd == -1)
		{
			printf("inotify add watch %s return -1\n", argv[i]);
			return -1;
		}

		printf("using wd: %d watch %s\n", wd, argv[i]);
	}

	char buffer[BUFFER_LENGTH];
	while (true)
	{
		ssize_t number = read(inotifyFd, buffer, BUFFER_LENGTH);
		if (number == 0)
		{
			printf("read inotify fd return 0\n");
			continue;
		}
		else if (number == -1)
		{
			printf("read inotify fd return -1\n");
			break;
		}

		printf("read %ld bytes from inotify fd\n", (long)number);
		char* p = buffer;
		while (p < buffer + number)
		{
			struct inotify_event* event = (struct inotify_event*)p;
			displayEvent(event);
			p += sizeof(struct inotify_event) + event->len;
		}
	}

	return 0;
}
