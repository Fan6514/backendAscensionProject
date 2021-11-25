#include <stdio.h>
#include <sys/epoll.h>

#include "epoll.h"
#include "common.h"

epoll_event *events;

int epollInit()
{
    int epoll_fd = 0;

    epoll_fd = epoll_create(MAX_EVENT);
    CHECK_RETURN_ERR(epoll_fd, -1, "epoll_create error.\n");

    GET_MEMORY_NORTN(events, epoll_event, sizeof(epoll_event)*MAX_EVENT);

    return epoll_fd;
}

int epollEventAdd(int epoll_fd, int fd)
{
    epoll_event event;
    int ret = SUCCESS;

    event.data.fd = fd;
    event.events = DEFAULT_EPOLL_EVENTS;

    /* 将 fd 绑定到 epoll_fd */
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    CHECK_RETURN_ERR(epoll_fd, -1, "epoll_ctl error.\n");

    return ret;
}

int epollEventMod(int epoll_fd, int fd, __uint32_t events)
{
    epoll_event event;
    int ret = SUCCESS;

    event.data.fd = fd;
    event.events = events;

    /* 修改 epoll_fd 中绑定的 fd 的事件为events */
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
    CHECK_RETURN_ERR(epoll_fd, -1, "epoll_ctl error.\n");

    return ret;
}

int epollEventDel(int epoll_fd, int fd, __uint32_t events)
{
    epoll_event event;
    int ret = SUCCESS;

    event.data.fd = fd;
    event.events = events;

    /* 删除 epoll_fd 中绑定的 fd */
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event);
    CHECK_RETURN_ERR(epoll_fd, -1, "epoll_ctl error.\n");

    return ret;
}
