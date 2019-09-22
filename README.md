README
========

记录学习 Linux 系统编程过程中一些系统接口的用法。
定位是 demo，所以代码写的比较随意，也会有重复代码。
重点是为了演示某些系统接口的基本用法。

基本上每个示例都是一个文件，可以独立编译和运行，但要使用 gcc 5 及以上版本。


LIST
==========

<details>
<summary>IO <code>file</code></summary>
<br>

* 获取文件大小
* 判断路径是否存在
</details>

<details>
<summary>IO <code>file</code></summary>
<br>

- [x] inotify
- [x] select / poll / epoll
- [x] pipe
- [x] popen
- [x] pwrite
- [x] stat
- [x] syslog
- [x] FILE <==> fd
</details>


<br>
<br>
<br>


- [ ] document
- [x] IO
	- [x] big file
	- [x] file lock(flock / fctrl)
	- [x] inotify
	- [x] select / poll / epoll
	- [x] pipe
	- [x] popen
	- [x] pwrite
	- [x] stat
	- [x] syslog
	- [x] FILE <==> fd
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
		- [x] clock / getrusage
	- [x] UNIX (real time and ticks)
		- [x] gettimeofday / time
		- [x] ctime_r / gmtime_r / localtime_r
		- [x] strftime / strptime
		- [x] times
	- [x] sleep
		- [x] sleep / usleep / nanosleep
		- [x] select / poll / epoll
		- [x] clock_nanosleep
- [x] timer
	- [x] POSIX
		- [x] timer_create / timer_delete
		- [x] timer_gettime / timer_settime / timer_getoverrun
	- [x] Linux
		- [x] timerfd_create / timerfd_gettime / timerfd_settime
- [ ] memory
- [ ] mmap
- [ ] thread
- [ ] process
- [ ] cmake
