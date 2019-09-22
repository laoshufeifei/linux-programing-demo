getrusage(RUSAGE_SELF, &uage);
Return resource usage statistics for the calling process,
which is the sum of resources used by all threads in the process.
包含子线程，但不包含子进程。

getrusage(RUSAGE_CHILDREN, &uage);
Return resource usage statistics for all children of the
calling process that have terminated and been waited for.
所有的子进程，但不包含自身。
如果是 fork 出来的子进程，必须 wait 之后再调用，否则不会统计出来。