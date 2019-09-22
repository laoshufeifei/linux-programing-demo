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
<br>

* 待完成
</details>


<details>
<summary>IO </summary>
<br>

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
</details>


<details>
<summary>directory </summary>
<br>

- [x] dirname / basename / realpath / getcwd
- [x] opendir / readdir / closedir
- [x] nftw(file tree walk)
</details>


<details>
<summary>socket </summary>
<br>
- [x] internet datagram / stream
- [x] UNIX domain datagram / stream
- [x] simple http client
</details>


<details>
<summary>signal </summary>
<br>

- [x] signal
- [x] sigaction
</details>


<details>
<summary>time </summary>
<br>

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


<details>
<summary>timer </summary>
<br>

- [x] POSIX
	- [x] timer_create / timer_delete
	- [x] timer_gettime / timer_settime / timer_getoverrun
- [x] Linux
	- [x] timerfd_create / timerfd_gettime / timerfd_settime
</details>


<details>
<summary>memory </summary>
<br>

</details>


<details>
<summary>mmap </summary>
<br>

</details>


<details>
<summary>thread </summary>
<br>

</details>


<details>
<summary>process </summary>
<br>

</details>


<details>
<summary>cmake </summary>
<br>

</details>

