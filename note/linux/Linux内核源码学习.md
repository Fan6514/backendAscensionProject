# Linux内核源码学习

Linux系统是目前服务器中比较流行的操作系统，在工作中也是以Linux为平台开发网络操作系统软件，因此对Linux系统需要更深入的了解。

本文参考Linux内核2.6.0版本学习，参考书目《深入理解LINUX内核 第三版》。

## 进程管理

进程：程序执行的一个实例，在Linux源码中常把进程称为**任务**（task）或线程（thread）。从内核观点来看，进程的目的是担当分配系统资源（CPU时间、内存等）的实体。

### 进程描述符

进程描述符：Linux管理进程所需的要的一些描述，就是进程在运行过程的一些状态信息，在源码中定义为`task_struct`结构。

```cpp
struct task_struct {
	volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped 进程状态 */
	struct thread_info *thread_info;
	atomic_t usage;
	pid_t pid;
	pid_t tgid;
    ...
};
```

上面给出的进程描述符的字段主要包括进程的主要信息：进程状态和进程的父子关系，还有其他字段，包括进程的基本信息 `thread_info` 一些进程所拥有的特殊资源。 

能被独立调度的每个上下文切换都有独立的进程描述符，每个进程和进程描述符都是严格一一对应关系，这里使用32位进程描述符地址来表示进程，存放在 `pid` 字段，新创建进程的PID是前一个进程的PID+1，其上限为32767（`PID_MAX_DEFAULT-1`），该值可在 `/proc/sys/kernel/pid_max` 修改一个更小的值。64位体系架构可以修改到4194303。

由于需要循环使用PID，因此内核通过管理 `pidmap_array` 位图来表示已分配的PID和闲置的PID号。

```cpp
typedef struct pidmap {
	atomic_t nr_free;
	void *page;     /* 一个页框32768个位，位图存放在一个单独的页中 */
} pidmap_t;

static pidmap_t pidmap_array[PIDMAP_ENTRIES] =
	 { [ 0 ... PIDMAP_ENTRIES-1 ] = { ATOMIC_INIT(BITS_PER_PAGE), NULL } };
```

此外，对于多线程应用所有的线程都必须有相同的PID（POSIX 1003.1c），Linux引入线程组，一个线程组的所有线程使用和该线程组的领头线程（thread group leader）相同的PID，这个值存放在字段 `tgid` 中。因此使用系统调用 `getpid()` 返回的是 `tgid` 的值而不是 `pid` 的值。