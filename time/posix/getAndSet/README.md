# call clock_gettime
CLOCK_REALTIME				/// realtime clock
							/// 真实时间，如果系统时间在统计过程中被改变了会影响准确性

CLOCK_MONOTONIC				/// Clock that cannot be set and represents monotonic time since 
							/// some unspecified starting point.
							/// 相对时间，修改系统时间不会影响准确性，推荐使用

CLOCK_PROCESS_CPUTIME_ID	/// High-resolution per-process timer from the CPU.
							/// 统计进程的用户态的 CPU 时间，不包含子进程

CLOCK_THREAD_CPUTIME_ID		/// Thread-specific CPU-time clock.
							/// 统计线程的用户态的 CPU 时间