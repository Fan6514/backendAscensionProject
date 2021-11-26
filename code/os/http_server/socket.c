#include <stdio.h>
#include <sys/socket.h>

#include "socket.h"
#include "common.h"

int socketInit(HTTP_SOCKET* http_socket, u_short *port)
{
    int on = 1;
    int ret = SUCCESS;

    CHECK_POINT(http_socket);
    CHECK_POINT(port);

    memset(http_socket, 0, sizeof(HTTP_SOCKET));
}

int socketBind(HTTP_SOCKET* http_socket)
{
    int ret = SUCCESS;

    CHECK_POINT(http_socket);

    ret = bind(http_socket->listen_fd, (struct sockaddr*)&http_socket->serverAddr,
                 sizeof(http_socket->serverAddr));
    CHECK_RETURN_ERR(ret, -1, "bind error.\n");

    return ret;
}

int socketListen(HTTP_SOCKET* http_socket)
{
    int ret = SUCCESS;

    CHECK_POINT(http_socket);

    ret = listen(http_socket->listen_fd, MAX_CONNECTION);
    CHECK_RETURN_ERR(ret, -1, "listen error.\n");

    return ret;
}

int socketAccept(HTTP_SOCKET* http_socket)
{
    CHECK_POINT(http_socket);

    http_socket->conn_fd = accept(http_socket->listen_fd, (struct sockaddr*)&http_socket->clientAddr, 
                                    sizeof(http_socket->clientAddr));
    CHECK_RETURN_ERR(http_socket->conn_fd, -1, "listen error.\n");
    
    return http_socket->conn_fd == -1 ? -1 : SUCCESS;
}

void socketUninit(HTTP_SOCKET* http_socket)
{
    CHECK_POINT(http_socket);

    close(http_socket->listen_fd);
    close(http_socket->conn_fd);

    memset(http_socket, 0, sizeof(http_socket));
}

