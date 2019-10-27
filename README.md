README
========

记录学习 Linux 系统编程过程中一些系统接口的用法。
定位是 demo，所以代码写的比较随意，也会有重复代码。
重点是为了演示某些系统接口的基本用法。

基本上每个示例都是一个文件，可以独立编译和运行，但要使用 gcc 5 及以上版本。


LIST
==========

<details>
<summary>Document <code>TODO</code></summary>

* 待完成
</details>


<details open>
<summary>IO </summary>

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
- [x] tmpfile
- [x] fifo
</details>


<details open>
<summary>directory </summary>

- [x] dirname / basename / realpath / getcwd
- [x] opendir / readdir / closedir
- [x] nftw(file tree walk)
</details>


<details open>
<summary>socket </summary>

- [x] internet datagram / stream
- [x] UNIX domain datagram / stream
- [x] simple http client
</details>


<details open>
<summary>signal </summary>

- [x] signal
- [x] sigaction
</details>


<details open>
<summary>time</summary>

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
</details>


<details open>
<summary>timer </summary>

- [x] POSIX
	- [x] timer_create / timer_delete
	- [x] timer_gettime / timer_settime / timer_getoverrun
- [x] Linux
	- [x] timerfd_create / timerfd_gettime / timerfd_settime
</details>


<details open>
<summary>memory </summary>

- [x] basic
	- [x] alloca / malloc / free
	- [x] sbrk
	- [x] memalign / posix_memalign
- [x] map file (shared or private)
</details>


<details open>
<summary>process </summary>

- [x] fork / getpid
- [x] system-v-ipc
	- [x] shared-memory
	- [x] semaphore
- [x] posix-ipc
	- [x] shared-memory
	- [x] semaphore
	- [x] message-queue
</details>


<details open>
<summary>thread </summary>

- [x] pthread_create / pthread_once / pthread_detach / pthread_join
- [x] pthread_cancel
- [x] Thread-local storage / Thread-specific Data
- [x] synchronization
	- [x] mutex
	- [x] condition
	- [x] RW-Lock
	- [x] spinlock
</details>


<details>
<summary>other </summary>

- [x] cmake
- [x] libc IO
</details>
