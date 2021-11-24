条件变量是一种线程同步机制，允许多个线程以无竞争的方式等待特定条件（条件不满足时线程挂起并等待）。注意**条件变量是用来等待线程**的，而不是上锁的，条件变量通常与互斥锁一起使用。

条件本身由互斥量保护，线程在改变条件状态之前必须先**锁住互斥量**，当条件满足时，线程通常解锁并等待条件发送变化，一旦另一个线程修改了环境变量，就会通知相应的环境变量唤醒一个或多个被条件变量阻塞的线程。唤醒后的线程将重新上锁，并测试条件是否满足。

## 1. 初始化条件变量

使用条件变量之前需要先对其进行初始化。初始化的方法有两种，一种是静态初始化，直接将 `PTHREAD_COND_INITIALIZER` 赋值给静态分配的条件变量；还有一种是动态分配，使用函数 `pthread_cond_init` 对其进行初始化。

```c
#include <pthread.h>
// 动态初始化
int pthread_cond_init(pthread_cond_t *restrict cond,
                      const pthread_condattr_t *restrict attr);
// 静态初始化
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// 反初始化
int pthread_cond_destroy(pthread_cond_t *cond);
```

其中 `attr` 参数为条件变量属性，通常设置为 NULL。

在释放条件变量内存空间前，使用 `pthread_cond_destroy` 函数对条件变量进行反初始化。

## 2. 阻塞等待条件变量

使用 `pthread_cond_wait` 等待条件变为真，如果特定时间内条件不满足，返回一个错误码的变量。

```c
#include <pthread.h>
int pthread_cond_wait(pthread_cond_t *restrict cond,
                      pthread_mutex_t *restrict mutex);
int pthread_cond_timewait(pthread_cond_t *restrict cond,
                      pthread_mutex_t *restrict mutex,
                      const struct timespec *restrict tsptr);
```

《APUE》书中对 `pthread_cond_wait` 函数描述：“调用者把**锁住的互斥量传给函数**，函数然后自动把调用线程放到等待条件的线程列表上，**对互斥量解锁。**这就关闭了条件检查和线程进入休眠状态等待条件改变这两个操作之间的时间通道，这样线程就不会错过条件的任何变化。**pthread_cond_wait返回时，互斥量再次被锁住。**”

关于这句话的分析可以参考博客：[https://blog.csdn.net/shichao1470/article/details/89856443](https://blog.csdn.net/shichao1470/article/details/89856443)。简单来说，该函数的功能是：

1. 阻塞等待条件变量 cond 变化；
2. 释放互斥量 mutex；
3. 条件变化后唤醒线程，函数返回时重新对互斥量 mutex 加锁。

此外，在使用该函数时可能还会出现条件变量的**虚假唤醒**问题，我们在后面继续讨论这个问题。

## 3. 唤醒条件变量上的线程

有两种方式唤醒线程，一种是使用 `pthread_cond_signal` 函数能唤醒至少一个等待该条件的线程，使用 `pthread_cond_broadcast` 函数能唤醒所有等待该条件的线程。

```c
#include <pthread.h>
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_bradcast(pthread_cond_t *cond);
```

**注意**一定要在改变条件状态以后再给线程发信号。

## 4. 应用实例

```c
#include <pthread.h>

struct msg {
  struct msg *m_next;
};

struct msg *workq;
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void process_msg()
{
  struct msg *mp;
  for (;;) {
    pthread_mutex_lock(&qlock);
    while (workq == NULL) {
      pthread_cond_wait(&qread, &qlock);
    }
    mp = workq;
    workq = mp->m_next;
    pthrad_mutex_unlock(&qlock);
  }
}

void enqueue_msg(struct msg *mp)
{
  pthread_mutex_lock(&qlock);
  mp->m_next = workq;
  workq = mp;
  pthread_mutex_unlock(&qlock);
  pthread_cond_signal(&qready);
}
```

## 5. 虚假唤醒

`pthread_cond_wait` 函数必须放在 while 循环里面，加入不放在 while 循环中，正常情况下，`pthread_cond_wait` 因为条件不满足而挂起线程。当条件满足以后，调用 `pthread_cond_signal` 唤醒挂起的线程。但当条件变量可能在某些情况下也被唤醒，这时 `pthread_cond_wait` 继续向下执行，而条件并不满足。此时的唤醒称作“虚假唤醒”。为了避免虚假唤醒而做出无意义的动作，将 `pthread_cond_wait` 放在 while 循环中，即使被虚假唤醒，由于 while 条件也会导致线程继续挂起。