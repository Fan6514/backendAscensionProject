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
/*--------------------------------------------------*/
/* 数据结构 
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

    pthread_mutex_t mutexPool;  /* 线程池锁 */
    pthread_cond_t condPool;
    pthread_cond_t notEmpty;    /* 任务队列非空 */
}ThreadPool;

/*--------------------------------------------------*/
/* 线程库函数
*/
/*--------------------------------------------------*/
/* 建立并初始化线程池 */
int threadPoolCreate(ThreadPool *pThreadPool, int taskCapacity, int poolSize);
/* 销毁线程池 */
int threadPoolDestory(ThreadPool *pThreadPool);
/* 添加任务到线程池 */
int threadPoolAddTask(ThreadPool *pThreadPool, void(*func)(void*), void *arg);

/* 管理线程库函数 */
void manager(void* arg);
void work(void* arg);
#endif
