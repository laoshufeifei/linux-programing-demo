README
========

记录一些学习 Linux 编程 过程中系统接口的用法。
定位是 demo，所以代码写的比较随意，
重点是为了演示某些系统接口的基本用法。

基本上每个示例都是一个文件，可以独立编译和运行。


LIST
==========

- [ ] document
- [x] hello world
- [x] IO
	- [x] big file
	- [x] file lock(flock/fctrl)
	- [x] inotify
	- [x] select / poll / epoll
	- [x] pipe
	- [x] popen
	- [x] pwrite
	- [x] stat
	- [x] syslog
- [x] directory
	- [x] dirname / basename / realpath / getcwd
	- [x] opendir / readdir / closedir
	- [x] nftw(file tree walk)
- [x] socket
	- [x] internet datagram / stream
	- [x] UNIX domain datagram / stream
	- [x] simple http client
- [x] signal
	- [x] signal
	- [x] sigaction
- [x] time
	- [x] POSIX (real time and ticks)
		- [x] clock_gettime / clock_settime
	- [x] UNIX (real time and ticks)
		- [x] gettimeofday / time
		- [x] ctime_r / gmtime_r / localtime_r
		- [x] strftime/strptime
		- [x] times/clock
	- [ ] sleep / nanosleep
- [ ] timer
- [ ] memory
- [ ] memory
- [ ] thread
- [ ] process
- [ ] cmake
