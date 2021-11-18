#include "thread_pool.h"

int threadPoolCreate(ThreadPool *pThreadPool, int taskCapacity, int poolSize)
{
    int ret = 0;

    /* 初始化线程池 */
    GET_MEMORY(pThreadPool, ThreadPool, sizeof(ThreadPool), error);
    GET_MEMORY(pThreadPool->workIDs, pthread_t, sizeof(pthread_t)*poolSize, error);
    pThreadPool->poolSize = poolSize;

    /* 初始化任务队列 */
    GET_MEMORY(pThreadPool->taskList, Task, sizeof(Task)*taskCapacity, error);
    pThreadPool->taskCapacity = taskCapacity;
    pThreadPool->taskSize = 0;
    pThreadPool->taskFront = 0;
    pThreadPool->taskTail = 0;

    /* 创建线程 */
    ret = pthread_create(pThreadPool->managerID, NULL, manager, pThreadPool);
    CHECK_RETURN(ret, SUCCESS, "pthread_create error.\n");
    for (int i = 0; i < poolSize; i++)
    {
        ret = pthread_create(pThreadPool->workIDs[i], NULL, work, pThreadPool);
        CHECK_RETURN(ret, SUCCESS, "pthread_create error.\n");
    }

    /* 初始化锁 */


    return SUCCESS;
error:
    REL_MEMORY(pThreadPool);
    REL_MEMORY(pThreadPool->workIDs);
    REL_MEMORY(pThreadPool->taskList);
    return MEM_ERROR;
}