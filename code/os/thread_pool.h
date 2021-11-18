#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <string.h>
#include <pthread.h>

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define SUCCESS         0
#define PARA_ERROR      1
#define RTN_ERROR       2
#define MEM_ERROR       3

#define GET_MEMORY(ptr, type, size)\
do\
{\
    ptr = (type*)malloc(size);\
    if (NULL == ptr)\
    {\
        printf("Malloc error.\n");\
        return MEM_ERROR;\
    }\
    memset(ptr, 0, size);\
}while(0);

#define REL_MEMORY(ptr)\
do\
{\
    if (NULL != ptr)\
    {\
        free(ptr);\
    }\
}while(0);
/*--------------------------------------------------*/
/* 数据结构 
*/
/*--------------------------------------------------*/
typedef struct Task
{
    int taskID;
    void (*function)(void* arg);
    void* arg;
}Task;

typedef struct ThreadPool
{
    Task *taskList;             /* 任务队列 */
    int taskCapacity;
    int taskSize;
    int taskFront;
    int taskTail;

    pthread_t managerID;
    pthread_t workIDs;
    int poolSize;
}ThreadPool;

/*--------------------------------------------------*/
/* 线程库函数
*/
/*--------------------------------------------------*/
/* 建立并初始化线程池 */
int threadPoolCreate(ThreadPool *pThreadPool, int taskCapacity, int poolSize);

#endif
