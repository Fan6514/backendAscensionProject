#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

int httpServerRun(int port, int pollSize, int pollCoreSize);
int httpServerStartUp(int port, int pollSize, int pollCoreSize, ThreadPool **ppThread_pool, 
                        int *epoll_fd, SERVER_SOCKET *server_socket);

#endif