#include <sys/socket.h>
#include <netinet/in.h>

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/

/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef struct httpSocket
{
    int listenFd;
    int connFd;
    socklen_t clientLen;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
}HTTP_SOCKET;

/*--------------------------------------------------*/
/* HTTP 函数
*/
/*--------------------------------------------------*/