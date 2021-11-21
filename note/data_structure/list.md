# 链表的实现 —— 以Linux内核2.6版本为例

数组和链表是计算机中最基础的两种存储数据的方式：顺序存储和链式存储。因此这两种是最基础的数据结构。

其中，链表的主要结构包括两部分，一部分用来存储当前节点的数据内容，另一部分存储连接该节点的其他节点地址。

数组链表的主要算法技巧是双指针，双指针⼜分为中间向两端扩散的双指针、两端向中间收缩的双指针、快慢指针。此外，数组还有前缀和和差分数组也属于必知必会的算法技巧。

本文以Linux内核中实现的双向链表来学习，不仅能学习到链表的结构，而且还可以了解到一些C语言的技巧。最后，

## 链表的实现

### 初始化链表

双向链表的数据结构中包括两个指针，分别指向当前节点的下一个节点和前一个节点。

```c
struct list_head {
	struct list_head *next, *prev;
};
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
