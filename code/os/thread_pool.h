#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*--------------------------------------------------*/
/* 宏定义
*/
/*--------------------------------------------------*/
#define SUCCESS         0
#define PARA_ERROR      1
#define RTN_ERROR       2
#define MEM_ERROR       3

#define PROCESS_THREAD_NUM   10

/* Define a Boolean data type. (from linux kernel 2.6.0) */
typedef enum { false, true } __attribute__ ((packed)) boolean;

#define GET_MEMORY(ptr, type, size, pos)\
do\
{\
    ptr = (type*)malloc(size);\
    if (NULL == ptr)\
    {\
        printf("Malloc error.\n");\
        goto pos;\
    }\
    memset(ptr, 0, size);\
}while(0)

#define REL_MEMORY(ptr)\
do\
{\
    if (NULL != ptr)\
    {\
        free(ptr);\
    }\
}while(0)

#define CHECK_RETURN(ret, sucess, str)\
do\
{\
    if (sucess != ret)\
    {\
        printf(str);\
    }\
}while(0)

#define CHECK_POINT(ptr)\
do\
{\
    if (NULL == ptr)\
    {\
        return MEM_ERROR;\
    }\
}while(0)

#define CHECK_POINT_NORTN(ptr)\
do\
{\
    if (NULL == ptr)\
    {\
        return;\
    }\
}while(0)
/*--------------------------------------------------*/
/* 数据结构 
 *      - 增加 minNum 属性让线程池根据任务
 *          数量在范围内动态变化
*/
/*--------------------------------------------------*/
typedef struct Task
{
    void (*function)(void* arg);
    void* arg;
}Task;

typedef struct ThreadPool
{
    Task *taskList;             /* 任务队列 */
    int taskCapacity;           /* 容量 */
    int taskSize;               /* 当前任务数量 */
    int taskFront;              /* 任务队列队首 */
    int taskTail;               /* 任务队列队尾 */

    pthread_t managerID;        /* 管理者线程id */
    pthread_t *workIDs;         /* 工作线程id */
    int poolSize;               /* 线程池大小 */
    int liveNum;                /* 存活线程数量 */
    int minNum;                 /* 线程最小数量 */
    int busyNum;                /* 忙线程数量 */
    int exitNum;                /* 销毁线程数量 */

    pthread_mutex_t mutexPool;  /* 线程池锁 */
    pthread_mutex_t mutexBusy;  /* 忙线程锁 */
    pthread_cond_t condPool;
    pthread_cond_t notEmpty;    /* 任务队列非空 */
    pthread_cond_t notFull;     /* 任务队列非满 */

    boolean isShutDown;         /* 线程池关闭标志 */
}ThreadPool;

/*--------------------------------------------------*/
/* 线程库函数
 *      工作线程函数
 *          - 从任务队列取任务然后调用
 *      管理线程函数
 *          - 当前线程数少于任务线程时创建新线程
 *          - 任务数过少时释放线程
*/
/*--------------------------------------------------*/
/* 建立并初始化线程池 */
int threadPoolCreate(ThreadPool *pThreadPool, int taskCapacity, int poolSize, int poolMinSize);
/* 销毁线程池 */
int threadPoolDestroy(ThreadPool *pThreadPool);
/* 添加任务到线程池 */
int threadPoolAddTask(ThreadPool *pThreadPool, void(*func)(void*), void *arg);

/* 管理线程库函数 */
void *manager(void* arg);
void *work(void* arg);

/* 线程退出 */
void threadExit(ThreadPool *pThreadPool);

#endif
