# sleep

sleep 精度为秒级别的，并且在拦截 SIGINT 信号的情况下，如果多次 Ctrl + c，实际休眠时间可能会变长；

usleep 在被信号打断后，并不会返回剩余没有 sleep 的时间；

Android(miniSDK = 15) 下的 _POSIX_C_SOURCE = 199009，不过也可以使用 nanosleep
select 实现的 sleep 在 Android 上会影响性能，不过不容易发现，但是在其他 Linux 平台上没发现影响。
poll 的性能要比 select 好，在 Android 上也没发现对性能的影响。

nanosleep 需要定义 _POSIX_C_SOURCE 并且 _POSIX_C_SOURCE >= 199309L
nanosleep 的实现不是以信号来实现的该函数。所以可以与 alarm() 或 settimer() 混用
如果被信号打断会返回 -1，并且 errno 被设置为 EINTR，并将剩余时间存储在第二个结构体里。

nanosleep 还不够完美的原因就在于 remain 时间不一定是时钟间隔的整数倍，所以 nanosleep 每次重启都有
可能遇到取整错误，有可能 nanosleep 重启后休眠的时间比前一次调用返回的时间要长。(类似于sleep的问题)
而 clock_nanosleep 可以用 TIMER_ABSTIME 来避免这个问题，
当然 clock_nanosleep 也支持相对时间。

以上所有的 sleep 在使用中都可以被信号打断。
如果正在 sleep 的时候收到了信号，只有当程序注册了信号处理函数，并且选择不退出程序的时候，sleep 才需要重启;
如果信号处理函数选择了退出程序，或者没有注册信号处理函数，sleep 不会重启。