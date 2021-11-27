#include <stdio.h>
#include<errno.h>

#include "epoll.h"
#include "socket.h"
#include "../thread_pool/thread_pool.h"
#include "../include/common.h"

extern struct epoll_event *events;

void httpServerRequest(void* arg)
{

}

int httpServerStartUp(int port, int pollSize, int pollCoreSize, ThreadPool **ppThread_pool, 
                        int *epoll_fd, SERVER_SOCKET *server_socket)
{
    int ret = SUCCESS;

    CHECK_POINT(ppThread_pool);
    CHECK_POINT(*ppThread_pool);
    CHECK_POINT(epoll_fd);
    CHECK_POINT(server_socket);

    /* 初始化线程池 */
    *ppThread_pool = threadPoolCreate(MAX_CONNECTION, pollSize, pollCoreSize);
    CHECK_POINT(*ppThread_pool);

    /* 初始化 socket */
    ret = socketInit(server_socket, port);
    CHECK_RETURN_ERR(ret, -1, "socketInit error.\n");

    /* 初始化 epoll */
    *epoll_fd = epollInit();
    CHECK_RETURN_ERR(*epoll_fd, -1, "epollInit error.\n");
    ret = epollEventAdd(*epoll_fd, server_socket->listen_fd);
    CHECK_RETURN_ERR(ret, -1, "epollEventAdd error.\n");

    return ret;
}

int httpServerRun(int port, int pollSize, int pollCoreSize)
{
    int ret = SUCCESS;
    int epoll_fd = 0;
    int eventSum = 0;
    SERVER_SOCKET server_socket;
    ThreadPool *thread_pool = NULL;

    /* 服务器初始化 */
    ret = httpServerStartUp(port, pollSize, pollCoreSize, &thread_pool, &epoll_fd, &server_socket);
    CHECK_RETURN_GOTO(ret, SUCCESS, "httpServerStartUp error.\n", error);

    while(true)
    {
        ret = epollWait(epoll_fd, &eventSum, events);
        if (SUCCESS != ret)
        {
            /* 信号中断或超时 */
            if (errno == EINTR || eventSum == 0) { continue; }
            else { break; }
        }
        
        for (int i = 0; i < eventSum; ++i)
        {
            ret = threadPoolAddTask(thread_pool, httpServerRequest, (void*)&server_socket);
            CHECK_RETURN(ret, SUCCESS, "threadPoolAddTask error.\n");
        }
    }

    return ret;

error:
    socketUninit(&server_socket);
    return ret;
}