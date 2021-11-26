#include <sys/socket.h>
#include <netinet/in.h>

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define MAX_CONNECTION 1024
/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef struct serverSocket
{
    int listen_fd;
    int conn_fd;
    socklen_t clientLen;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    u_short port;
}SERVER_SOCKET;

/*--------------------------------------------------*/
/* SOCKET 函数
*/
/*--------------------------------------------------*/
int socketInit(HTTP_SOCKET* http_socket, u_short *port);
int socketBind(HTTP_SOCKET* http_socket);
int socketListen(HTTP_SOCKET* http_socket);
int socketAccept(HTTP_SOCKET* http_socket);
void socketUninit(HTTP_SOCKET* http_socket);
