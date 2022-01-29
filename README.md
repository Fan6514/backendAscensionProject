 <h1 align="center">&#x1F4DC 程序员后端技术栈精进计划 </h1>
> 一切伟大的行动和思想，都有一个微不足道的开始。 

这里分享一些看过的文章并且总结起来，让平时碎片化学习的知识体系化，以便之后可以反复学习。

## 一、:fire: 语言基础篇

**1.1 指针与内存**

-   [指针使用必知必会](note/c_cpp/pointer.md)
-   [预处理器使用指南](note/c_cpp/include.md)
-   [深入理解链接器](note/c_cpp/linker.md)
-   [C/C++ 内存管理](note/c_cpp/memory.md)

**1.2 编程规范**

-   [结构体内存的自然对齐](note/c_cpp/struct_memory.md)

**1.3 面向对象编程**

-   [多线程下的析构函数](note/c_cpp/destructor.md)

## 二、:fire:  网络编程篇

**2.1 计算机网络基础**

**2.2 网络编程模型**

-   进程间通信方式

-   多线程编程

-   五大 IO 模型

-   高性能 IO 两种模式：Reactor 和 Proactor

-   [IO 多路复用机制](note/net/epoll.md)

**2.3 项目实践**

-   [:bulb:http服务器项目](https://github.com/Fan6514/httpServer4c)

-   网络库（one loop per thread + thread pool 模式）

-   RPC

-   网络聊天室

## 三、:fire: 操作系统篇

**3.1 系统启动**

-   [系统上电启动](note/os/boost.md)

**3.2 进程管理**

-   进程与线程管理
-   [Linux内核源码学习之进程管理](note/linux/Linux内核源码学习.md)
-   [多线程必备技能：条件变量与互斥锁](note/os/cond.md)
-   [高并发中的线程与线程库的实现](note/os/threadpool.md)

**3.3 内存管理**

-   内存管理

**3.4 文件I/O系统**

-   [程序 I/O 操作详解](note/linux/io.md)

## 四、:fire: 算法篇

**4.1 初识算法**

-   [认识复杂度、对数器和异或运算](note/data_structure/base.md)
-   [认识二分法](note/data_structure/binary.md)

**4.2 数据结构**

-   [基本数据结构：链表](note/data_structure/list.md)
-   [栈与队列](note/data_structure/stack_queue.md)

**4.3 排序算法**

-   [排序算法的实现](note/data_structure/sort.md)
-   字符串匹配算法：KMP

常见算法思想：递归、枚举、递推、分治、贪心、动态规划等

## 五、:fire: 数据库篇

索引存储结构：B树、B+树索引、Hash索引

索引的使用：主键索引、覆盖索引、最左前缀原则、索引下推等

锁：乐观锁、悲观锁、表锁，行锁，意向锁，读锁，写锁等等

MySQL InnoDB MVCC 实现机制

存储引擎：InnoDB、MyISAM等，各自的优缺点

事务：ACID理论

## 六、:fire: 系统设计架构篇

**6.1 基础架构模式**

-   [常用的十种软件架构模式](note/system_design/sys_design_structure.md)

## 七、:fire: 虚拟化与云计算篇

## 八、:fire: 人工智能篇

## 九、:fire: 项目相关技能篇

