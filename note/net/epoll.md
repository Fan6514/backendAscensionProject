## IO 多路复用机制

在 Linux 中，一切 IO 设备都被抽象成文件，磁盘、网络数据、终端，甚至进程间通信工具管道pipe等都被当做文件对待。所有的I/O操作也都可以通过文件读写来实现，**这一非常优雅的抽象可以让程序员使用一套接口就能对所有外设I/O操作**。

常用的I/O操作接口一般有以下几类：

-   打开文件，open

-   改变读写位置，seek

-   文件读写，read、write

-   关闭文件，close

程序员通过这几个接口几乎可以实现所有I/O操作，这就是文件这个概念的强大之处。

**文件描述符**：简称 fd，当应用程序请求内核打开/新建一个文件时，内核会返回一个文件描述符用于对应这个打开/新建的文件，其 fd 本质上就是一个非负整数，读写文件也是需要使用这个文件描述符来指定待读写的文件的。

**有了文件描述符，进程可以对文件一无所知**，比如文件在磁盘的什么位置、加载到内存中又是怎样管理的等等，这些信息统统交由操作系统打理，进程无需关心，操作系统只需要给进程一个文件描述符就足够了。 

所有I/O操作都可以通过文件样的概念来进行，这当然包括网络通信。例如 web 服务器，当三次握手成功后，我们会调用 accept 来获取一个链接，调用该函数我们同样会得到一个文件描述符 socket，通过这个文件描述符就可以处理客户端发送的请求并且把处理结果发送回去。

```c
// 通过accept获取客户端的文件描述符
int conn_fd = accept(...);
```

server的处理逻辑通常是读取客户端请求数据，然后执行某些特定逻辑：

```c
if (read(conn_fd, request_buff) > 0)
{
    do_something(request_buff);
}
```

对于高并发服务器，同时会有成千上万个客户端进行通信，这时就要处理成千上万的文件描述符。

典型的阻塞式I/O，如果此时没有数据可读那么**进程会被阻塞而暂停运行**，这时我们就无法处理第二个请求了，即使第二个请求的数据已经就位，这也就意味着处理某一个客户端时由于进程被阻塞导致剩下的所有其它客户端必须**等待**，在同时处理几万客户端的server上，这显然是不能容忍的。

**IO 多路复用（ I/O multiplexing）**

相比 IO 接口**主动**问内核这些文件描述符对应的外设是不是已经就绪了，一种更好的方法是，我们把这些感兴趣的文件描述符一股脑扔给内核，由内核进行监视，如果有可读写的文件描述符就通知用户来处理。

I/O 多路复用指的是这样一个过程：

-   拿到了一堆文件描述符(不管是网络相关的、还是磁盘文件相关等等，任何文件描述符都可以) 
-   通过调用**某个函数**告诉内核：“**这个函数你先不要返回，你替我监视着这些描述符，当这堆文件描述符中有可以进行I/O读写操作的时候你再返回**” 
-   当调用的这个函数返回后我们就能知道哪些文件描述符可以进行I/O操作了。 

 **通过 I/O 多路复用我们可以同时处理多路 I/O**。在 Linux 中有这样三种机制可以用来进行I/O多路复用： select、poll、epoll。

###  **I/O多路复用三剑客** 

本质上 select、poll、epoll 都是阻塞式 I/O，也就是我们常说的同步 I/O，原因在于调用这些 I/O 多路复用函数时如果任何一个需要监视的文件描述符都不可读或者可写那么进程会被阻塞暂停执行，直到有文件描述符可读或者可写才继续运行。

**1. select：初出茅庐**

在 select 这种 I/O 多路复用机制下，我们需要把想监控的文件描述集合通过函数参数的形式告诉 select，然后 select 会将这些文件描述符集合**拷贝**到内核中，我们知道数据拷贝是有性能损耗的，因此为了减少这种数据拷贝带来的性能损耗，Linux 内核对集合的大小做了限制，并规定用户监控的文件描述集合不能超过 1024 个，同时当 select 返回后**我们仅仅能知道有些文件描述符可以读写了，但是我们不知道是哪一个**，因此程序员必须再遍历一边找到具体是哪个文件描述符可以读写了。

因此，总结下来 select 有这样几个特点：

-   我能照看的文件描述符数量有限，不能超过 1024 个
-   用户给我的文件描述符需要拷贝的内核中
-   我只能告诉你有文件描述符满足要求了，但是我不知道是哪个，你自己一个一个去找吧(遍历)

select 机制的这些特性在高并发网络服务器动辄几万几十万并发链接的场景下无疑是低效的。 

**2. poll：小有所成**

poll 和 select 是非常相似的，poll 相对于 select 的优化仅仅在于解决了文件描述符不能超过 1024 个的限制，select 和 poll 都会随着监控的文件描述数量增加而性能下降，因此不适合高并发场景。 

**3. epoll：独步天下**

文件描述数量限制已经在 poll 中解决了。针对拷贝问题，epoll使用的策略是**各个击破**与**共享内存**。

 epoll 通过引入 epoll_ctl 做到了只操作那些有变化的文件描述符，同时 epoll 和内核还共享了同一块内存，这块内存中保存的就是那些已经可读或者可写的的文件描述符集合，这样就减少了内核和程序的拷贝开销。 

epoll 采用**事件驱动机制**，进程不需要一遍一遍地询问各个文件描述符，而是文件描述符有哪个可读写主动上报。

### epoll 调用函数

epoll 只有`epoll_create`、`epoll_ctl`和`epoll_wait`这三个系统调用。其定义如下： 

```c
#include <sys/epoll.h>

int epoll_create(int max_size);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

`epoll_create`  方法创建一个 epoll 的句柄，max_size传入最大监听数目，从Linux 2.6.8 开始，max_size参数将被忽略，但必须大于零。

`epoll_ctl` 方法注册要监听的事件类型。其中：

-   参数 `epfd` 为创建的 epoll 句柄
-   参数 `op` 表示动作类型，有三个宏来表示：
    -   ` EPOLL_CTL_ADD `：注册新的 fd 到 epfd 中；
    -   ` EPOLL_CTL_MOD `：修改已经注册的 fd 的监听事件；
    -   ` EPOLL_CTL_DEL `：从 epfd 中删除一个 fd。 
-   参数 `fd` 表示需要监听的文件
-   参数 `event` 表示要监听的事件，其数据结构为：

```c
// 保存触发事件的某个文件描述符相关的数据
typedef union epoll_data {
    void *ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;

// 感兴趣的事件和被触发的事件
struct epoll_event {
    __uint32_t events; // Epoll events
    epoll_data_t data; // User data variable
};
```

对于Epoll Events，其可以是以下几个宏的集合：

-   `EPOLLIN`：表示对应的文件描述符可读（包括对端Socket）；
-   `EPOLLOUT`：表示对应的文件描述符可写；
-   `EPOLLPRI`：表示对应的文件描述符有紧急数据可读（带外数据）；
-   `EPOLLERR`：表示对应的文件描述符发生错误；
-   `EPOLLHUP`：表示对应的文件描述符被挂断；
-   `EPOLLET`：将EPOLL设为边缘触发（Edge Triggered），这是相对于水平触发（Level Triggered）而言的。
-   `EPOLLONESHOT`：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket，需要再次

`epoll_wait` 方法收集在epoll监控的事件中已经发生的事件。参数 `events` 是分配好的 `epoll_event` 结构体数组，epoll 将会把发生的事件赋值到 events 数组中（events 不可以是空指针，内核只负责把数据赋值到这个 event 数组中，不会去帮助我们在用户态分配内存）。`maxevents` 告诉内核这个 events 数组有多大，这个 maxevents 的值不能大于创建epoll_create时的 size。参数 timeout 是超时时间（毫秒）。如果函数调用成功，则返回对应 IO 上已准备好的文件描述符数目，如果返回 0 则表示已经超时。

### epoll 工作模式

**LT 模式**

Level Triggered，水平触发。 该模式是 epoll 的缺省工作模式，其同时支持阻塞和非阻塞 socket。内核会告诉开发者一个文件描述符是否就绪，如果开发者不采取任何操作，内核仍会一直通知。 

**ET模式**

该模式是一种高速处理模式，当且**仅当状态发生变化时才会获得通知**。在该模式下，其假定开发者在接收到一次通知后，会完整地处理该事件，因此内核将不再通知这一事件。注意，缓冲区中还有未处理的数据不能说是状态变化，因此，在 ET 模式下，开发者如果只读取了一部分数据，其将再也得不到通知了。正确的做法是，开发者自己确认读完了所有的字节（一直调用 read/write直到出错 EAGAGIN 为止）。

Nginx 默认采用的就是ET（边缘触发）。

```c
#define MAX_EVENT 1024
#define TIME_OUT 1000
#define DEFAULT_EPOLL_EVENTS (EPOLLIN | EPOLLET)	// ET模式

// 初始化 epoll 句柄
int epollInit()
{
    int epoll_fd = 0;

    epoll_fd = epoll_create(MAX_EVENT);
    CHECK_RETURN_ERR(epoll_fd, -1, "epoll_create error.");

    GET_MEMORY_RTN(events, struct epoll_event, sizeof(struct epoll_event)*MAX_EVENT);

    return epoll_fd;
}

// 添加事件
int epollEventAdd(int epoll_fd, int fd)
{
    struct epoll_event event;
    int ret = SUCCESS;

    event.data.fd = fd;
    event.events = DEFAULT_EPOLL_EVENTS;

    /* 将 fd 绑定到 epoll_fd */
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    CHECK_RETURN_ERR(epoll_fd, -1, "epoll_ctl error.");

    return ret;
}

int epollWait(int epoll_fd, int *eventSum, struct epoll_event *epoll_events)
{
    int ret = SUCCESS;

    CHECK_POINT(eventSum);
    CHECK_POINT(epoll_events);

    *eventSum = epoll_wait(epoll_fd, epoll_events, MAX_EVENT, TIME_OUT);
    CHECK_RETURN_ERR(*eventSum, -1, "epoll_wait error.");
    ret = *eventSum == -1 ? RTN_ERROR : SUCCESS;

    return ret;
}
```

### epoll 高性能探讨

epoll的高效性主要体现在以下三个方面：

（1）select/poll 每次调用都要传递所要监控的所有 fd 给 select/poll 系统调用，这意味着每次调用 select/poll 时都要**将 fd 列表从用户空间拷贝到内核**，当 fd 数目很多时，这会造成性能低效。

对于 epoll_wait，每次调用 epoll_wait 时，其不需要将 fd 列表传递给内核，epoll_ctl 不需要每次都拷贝所有的 fd 列表，只需要进行增量式操作。因此，在调用 epoll_create 函数之后，内核已经在内核开始准备数据结构用于存放需要监控的 fd 了。其后，**每次 epoll_ctl 只是对这个数据结构进行简单的维护操作即可**。

（2）内核使用 slab 机制，为 epoll 提供了快速的数据结构。在内核里，一切都是文件。因此，epoll 向内核注册了一个文件系统，用于存储所有被监控的 fd。

当调用 epoll_create 时，就会在这个虚拟的 epoll 文件系统中创建一个 file 节点。epoll 在被内核初始化时，同时会分配出 epoll 自己的内核告诉 cache 区，用于存放每个我们希望监控的 fd。

这些 fd 会以**红黑树的形式保存在内核 cache 里，以支持快速查找、插入和删除**。这个内核高速 cache，就是建立连续的物理内存页，然后在之上建立 slab 层，简单的说，就是物理上分配好想要的 size 的内存对象，每次使用时都使用空闲的已分配好的对象。

（3）当调用 epoll_ctl 往 epfd 注册百万个 fd 时，epoll_wait 仍然能够快速返回，并有效地将发生的事件 fd 返回给用户。原因在于，当我们调用 epoll_create 时，内核除了帮我们在 epoll 文件系统新建 file 节点，同时在内核 cache 创建红黑树用于存储以后由 epoll_ctl 传入的 fd 外，还会再建立一个 list 链表，用于存储准备就绪的事件。当调用 epoll_wait 时，仅仅观察这个 list 链表中有无数据即可。

如果 list 链表中有数据，则返回这个链表中的所有元素；如果 list 链表中没有数据，则 sleep 然后等到 timeout 超时返回。所以，epoll_wait 非常高效，而且，通常情况下，即使我们需要监控百万计的 fd，但大多数情况下，一次也只返回少量准备就绪的 fd 而已。因此，每次调用 epoll_wait，其仅需要从内核态复制少量的 fd 到用户空间而已。

那么，这个准备就绪的list 链表是怎么维护的呢？过程如下：当我们执行 epoll_ctl 时，除了把 fd 放入到 epoll 文件系统里 file 对象对应的红黑树之外，还会给内核中断处理程序注册一个回调函数，其告诉内核，如果这个 fd 的中断到了，就把它放到准备就绪的 list 链表中。

### Linux epoll 的实现机制

![](/note/net/pics/epoll.webp)

Linux poll 实现的逻辑如下：

1. 创建epoll句柄，初始化相关数据结构
2. 为epoll句柄添加文件句柄，注册睡眠entry的回调
3. 事件发生，唤醒相关文件句柄睡眠队列的entry，调用其回调
4. 唤醒epoll睡眠队列的task，搜集并上报数据

**创建epoll句柄，初始化相关数据结构**

主要就是创建一个 epoll 文件描述符，注意，后面操作 epoll 的时候，就是用这个 epoll 的文件描述符来操作的，所以这就是 epoll 的句柄，精简过后的 epoll 结构如下：

```c
 struct eventpoll {
 	// 阻塞在epoll_wait的task的睡眠队列
 	wait_queue_head_t wq;
 	// 存在就绪文件句柄的list，该list上的文件句柄事件将会全部上报给应用
 	struct list_head rdllist;
 	// 存放加入到此epoll句柄的文件句柄的红黑树容器
 	struct rb_root rbr;
 	// 该epoll结构对应的文件句柄，应用通过它来操作该epoll结构
 	struct file *file;
};
```

**为 epoll 句柄添加文件句柄，注册睡眠 entry 的回调**

**添加文件句柄**：将一个文件句柄，比如 socket 添加到 epoll 的 rbr 红黑树容器中，注意，这里的文件句柄最终也是一个包装结构，和 epoll 的结构体类似：

```c
struct epitem {
    // 该字段链接入epoll句柄的红黑树容器
    struct rb_node rbn;
    // 当该文件句柄有事件发生时，该字段链接入“就绪链表”，准备上报给用户态
    struct list_head rdllink;
    // 该字段封装实际的文件，我已经将其展开
    struct epoll_filefd {
        struct file *file;
        int fd;
    } ffd;
    // 反向指向其所属的epoll句柄
    struct eventpoll *ep;
};
```

以上结构将被添加到epoll的rbr容器中的逻辑如下：

```c
struct eventpoll *ep = 待加入文件句柄所属的epoll句柄;
struct file *tfile = 待加入的文件句柄file结构体;
int fd = 待加入的文件描述符ID;

struct epitem *epi = kmem_cache_alloc(epi_cache, GFP_KERNEL);
INIT_LIST_HEAD(&epi->rdllink);
INIT_LIST_HEAD(&epi->fllink);
INIT_LIST_HEAD(&epi->pwqlist);
epi->ep = ep;
ep_set_ffd(&epi->ffd, tfile, fd);
...
ep_rbtree_insert(ep, epi);
```

**注册睡眠 entry 回调并 poll 文件句柄**：Linux 内核的 sleep/wakeup 机制非常重要，几乎贯穿了所有的内核子系统，值得注意的是，这里的 sleep/wakeup 依然采用了 OO 的思想，并没有限制睡眠的 entry 一定要是一个 task，而是将睡眠的 entry 做了一层抽象，即：

```c
struct __wait_queue {
    unsigned int flags;
    // 至于这个private到底是什么，内核并不限制，显然，它可以是task，也可以是别的。
    void *private;
    wait_queue_func_t func;
    struct list_head task_list;
};
```

### epoll 惊群问题

事件模型使程序阻塞在事件上而不是阻塞在事物上，也就是说内核仅仅通知**发生了某件事**，而具体发生了什么事，由处理的进程或线程自己 poll。这样事件模型就可以**一次搜索多个事件**，满足多路复用的需求。
