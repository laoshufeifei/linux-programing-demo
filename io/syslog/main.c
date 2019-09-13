#include<syslog.h>

int main(void)
{
	openlog("test", LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_LOCAL0);
	syslog(LOG_PERROR, "error message");
	syslog(LOG_USER | LOG_INFO, "info message");
	closelog();
}
