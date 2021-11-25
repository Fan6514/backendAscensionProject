#ifndef _EPOLL_H_
#define _EPOLL_H_

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_EVENT 1024
#define DEFAULT_EPOLL_EVENTS (EPOLLIN | EPOLLET | EPOLLONESHOT)

/*--------------------------------------------------*/
/* EPOLL 函数
*/
/*--------------------------------------------------*/
int epollInit();
int epollEventAdd(int epoll_fd, int fd);
int epollEventMod(int epoll_fd, int fd, __uint32_t events);
int epollEventDel(int epoll_fd, int fd, __uint32_t events);

#endif