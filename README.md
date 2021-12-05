# 程序员后端技术栈精进计划

程序员后端技术栈精进计划，记录后端学习内容。学习内容与计划来自：[编程指北 - Linux C/C++ 后台服务器开发学习路线](https://mp.weixin.qq.com/s/rVjn6GuCV0NFl2ZyVjxdyw)

## C/C++ 语言特性和实现原理

[指针、引用、数组、内存](note/c_cpp/pointer.md)

引用与指针区别

C 和 C++ 的一些区别，比如 new、delete 和 malloc、free 的区别

虚机制：虚函数、虚函数表、纯虚函数

继承、虚继承、菱形继承等

多态：动态绑定，静态多态

重写、重载

智能指针原理：引用计数、RAII（资源获取即初始化）思想

智能指针使用：shared_ptr、weak_ptr、unique_ptr等

一些关键字的作用：static、const、volatile、extern

四种类型转换：static_cast, dynamic_cast, const_cast, reinterpret_cast

STL部分容器的实现原理，如 vector、deque、map、hashmap

模板特化、偏特化，萃取 traits 技巧

编译链接机制、内存布局（memory layout）、对象模型

C++11 部分新特性，比如右值引用、完美转发等

# 网络编程

计算机网络知识

进程间通信方式

多线程编程

五大 IO 模型

高性能 IO 两种模式：Reactor 和 Proactor

IO 复用机制：epoll、select、poll（破解 C10K 问题的利器）

[http服务器项目](https://github.com/Fan6514/httpServer4c)

这里推荐两个做为参考：

https://github.com/imarvinle/WebServer ：编程指北写的

https://github.com/linyacool/WebServer ：这是牛客 linya 大佬写的

https://github.com/qinguoyi/TinyWebServer ：轻量级Linux下Web服务器

网络库（one loop per thread + thread pool 模式）

RPC

网络聊天室

# 操作系统原理

进程与线程管理

内存管理

IO与文件系统

设备管理

[多线程必备技能：条件变量与互斥锁](note/os/cond.md)

[线程库的实现](note/os/threadpool.md)

# Linux私房菜

# 算法与数据结构

[认识复杂度、对数器、二分法和异或运算](note/data_struct/base.md)

[基本数据结构：链表](note/data_structure/list.md)

栈与队列

树、二叉树、多叉树实现和遍历方式，AVL树实现以及插入删除过程、红黑树（了解定义即可）

图，以及图的实现方式、遍历

B树、B+树

堆

散列函数和散列表

[排序算法的实现](note/data_structure/sort.md)

字符串匹配算法：KMP

常见算法思想：递归、枚举、递推、分治、贪心、动态规划等

# 数据库使用及原理

索引存储结构：B树、B+树索引、Hash索引

索引的使用：主键索引、覆盖索引、最左前缀原则、索引下推等

锁：乐观锁、悲观锁、表锁，行锁，意向锁，读锁，写锁等等

MySQL InnoDB MVCC 实现机制

存储引擎：InnoDB、MyISAM等，各自的优缺点

事务：ACID理论

# 版本控制 Git

# 分布式相关

# 虚拟化和云计算

# 系统设计与架构
