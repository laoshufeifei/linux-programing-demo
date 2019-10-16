#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

static void _showAttribute(struct mq_attr* attr)
{
	if (attr == NULL)
		return;

	printf("show mq attribute: flags: %ld, maxMsg: %ld, msgSize: %ld, curMsgs: %ld\n",
		attr->mq_flags, attr->mq_maxmsg, attr->mq_msgsize, attr->mq_curmsgs);
}

int main()
{
	// name must start with '/'
	const char* mqName = "/mq_test";

	bool createNew = true;
	mqd_t mq = mq_open(mqName, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR, NULL);
	if (mq == (mqd_t)-1)
	{
		createNew = false;
		mq = mq_open(mqName, O_RDWR);
		printf("open an exist message queue\n");
	}
	else
	{
		printf("create an new message queue\n");
	}

	if (mq == (mqd_t)-1)
	{
		printf("mq_open failed, errno is %d\n", errno);
		return -1;
	}

	struct mq_attr attr;
	mq_getattr(mq, &attr);
	_showAttribute(&attr);

	if (attr.mq_curmsgs == 0)
	{
		int ret = mq_send(mq, "abc", 3, 0);
		printf("mq_send [abc] %s\n", ret == 0 ? "success" : "fail");
		// sleep(5);
	}
	else
	{
		char buffer[8192] = {0, };	// have relationship to attr.mq_msgsize
		int ret = mq_receive(mq, buffer, 8192, NULL);
		printf("mq_receive [%s] %s\n", buffer, ret != -1 ? "success" : "fail");
	}

	mq_close(mq);
	// mq_unlink(mqName);
	return 0;
}
