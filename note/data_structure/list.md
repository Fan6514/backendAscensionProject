# 链表的实现 —— 以Linux内核2.6版本为例

数组和链表是计算机中最基础的两种存储数据的方式：顺序存储和链式存储。因此这两种是最基础的数据结构。

其中，链表的主要结构包括两部分，一部分用来存储当前节点的数据内容，另一部分存储连接该节点的其他节点地址。

数组链表的主要算法技巧是双指针，双指针⼜分为中间向两端扩散的双指针、两端向中间收缩的双指针、快慢指针。此外，数组还有前缀和和差分数组也属于必知必会的算法技巧。

本文以Linux内核中实现的双向链表来学习，不仅能学习到链表的结构，而且还可以了解到一些C语言的技巧。

## 链表的实现

### 初始化链表

双向链表的数据结构中包括两个指针，分别指向当前节点的下一个节点和前一个节点。

```c
struct list_head {
    struct list_head *next, *prev;
};
```
一个链表不仅存储前后节点的地址，而且还有数据部分，因此在linux中定义的链表结构通常嵌入在其他结构体中，例如在进程描述符 task_struct 结构体中就使用了链表来表示正在运行的进程、子进程等。

```c
struct task_struct {
    volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped 进程状态 */
    struct thread_info *thread_info;

    struct list_head run_list;
    struct list_head children;	/* list of my children */
    struct list_head sibling;	/* linkage in my parent's children list */
```

链表初始化时链表为空，则头节点的前节点和后节点都指向自身。

```c
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)
```

以上链表初始化的实现中巧妙运用了宏函数，在创建链表的宏函数 `LIST_HEAD` 中，其实质就是创建了一个 `struct list_head` 的结构体，并赋值该节点地址给结构成员。

`INIT_LIST_HEAD` 宏函数用来初始化一个已经创建的链表。

### 添加节点

```c
static inline void __list_add(struct list_head *new,
                  struct list_head *prev,
                  struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}
/* 在head节点之后添加新节点 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}
/* 在head节点之前添加新节点 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}
```

将一个完整的链表添加到某节点之后：

```c
static inline void __list_splice(struct list_head *list,
                struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

static inline void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head);
}
```

### 删除节点

```c
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = LIST_POISON1;
    entry->prev = LIST_POISON2;
}
```

### 移动节点

```c
static inline void list_move(struct list_head *list, struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add(list, head);
}

static inline void list_move_tail(struct list_head *list,
                  struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add_tail(list, head);
}
```

### 链表判空

```c
static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

static inline int list_empty_careful(const struct list_head *head)
{
    struct list_head *next = head->next;
    return (next == head) && (next == head->prev);
}
```

### 获取节点结构体首地址

`list_entry` 函数是Linux中比较常见的函数，用来获取链表节点在 type 类型数据结构的结构体首地址，member 是 type 类型结构体成员。

```c
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})
```

函数参数：

- ptr：链表中的其中一个节点
- type：要寻找的结构体类型
- member：type类型结构体成员，该成员表示链表的节点

这里实际是使用了当前节点 ptr 的值减去结构体成员与结构体首地址的距离，得到该结构体的首地址。

### 遍历链表

```c
#define __list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
```

该函数通常与 `list_entry` 一起使用，例如在查找 pid 的时候：

```c
__list_for_each(elem, bucket) {
    pid = list_entry(elem, struct pid, hash_chain);
    if (pid->nr == nr)
        return pid;
```
