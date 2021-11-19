#include "thread_pool.h"

int threadPoolCreate(ThreadPool *pThreadPool, int taskCapacity, int poolSize)
{
    int ret = 0;

    /* 初始化线程池 */
    GET_MEMORY(pThreadPool, ThreadPool, sizeof(ThreadPool), error);
    GET_MEMORY(pThreadPool->workIDs, pthread_t, sizeof(pthread_t)*poolSize, error);
    pThreadPool->poolSize = poolSize;
    pThreadPool->liveNum = 0;

    /* 初始化任务队列 */
    GET_MEMORY(pThreadPool->taskList, Task, sizeof(Task)*taskCapacity, error);
    pThreadPool->taskCapacity = taskCapacity;
    pThreadPool->taskSize = 0;
    pThreadPool->taskFront = 0;
    pThreadPool->taskTail = 0;

    /* 初始化锁 */
    ret = pthread_mutex_init(&pThreadPool->mutexPool, NULL);
    CHECK_RETURN(ret, SUCCESS, "pthread_mutex_init error.\n");
    /* 初始化条件变量 */
    ret = pthread_cond_init(&pThreadPool->condPool, NULL);
    CHECK_RETURN(ret, SUCCESS, "pthread_cond_init error.\n");
    ret = pthread_cond_init(&pThreadPool->notEmpty, NULL);
    CHECK_RETURN(ret, SUCCESS, "pthread_cond_init error.\n");

    /* 创建线程 */
    ret = pthread_create(pThreadPool->managerID, NULL, manager, pThreadPool);
    CHECK_RETURN(ret, SUCCESS, "pthread_create error.\n");
    for (int i = 0; i < poolSize; i++)
    {
        ret = pthread_create(pThreadPool->workIDs[i], NULL, work, pThreadPool);
        CHECK_RETURN(ret, SUCCESS, "pthread_create error.\n");
    }

    return SUCCESS;

error:
    REL_MEMORY(pThreadPool);
    REL_MEMORY(pThreadPool->workIDs);
    REL_MEMORY(pThreadPool->taskList);
    return MEM_ERROR;
}

int threadPoolDestory(ThreadPool *pThreadPool)
{
    int ret = 0;

    CHECK_POINT(pThreadPool);

    /* 阻塞管理线程 */
    pthread_join(pThreadPool->managerID, NULL);
    /* 唤醒阻塞线程 */
    for (int i = 0; i < pThreadPool->liveNum; ++i)
    {
        pthread_cond_signal(&pThreadPool->notEmpty);
    }

    /* 销毁锁 */
    pthread_mutex_destroy(&pThreadPool->mutexPool);
    pthread_cond_destory(&pThreadPool->condPool);
    pthread_cond_destory(&pThreadPool->notEmpty);

    /* 释放内存 */
    REL_MEMORY(pThreadPool->workIDs);
    REL_MEMORY(pThreadPool->taskList);
    REL_MEMORY(pThreadPool);

    return SUCCESS;
}

int threadPoolAddTask(ThreadPool *pThreadPool, void(*func)(void*), void *arg)
{
    int ret = 0;

    CHECK_POINT(pThreadPool);
    CHECK_POINT(func);
    CHECK_POINT(arg);

    pthread_mutex_lock(&pThreadPool->mutexPool);

    /* 添加任务到任务队列 */
    pThreadPool->taskList[pThreadPool->taskTail].function = func;
    pThreadPool->taskList[pThreadPool->taskTail].arg = arg;
    pThreadPool->taskTail = pThreadPool->taskTail % pThreadPool->taskCapacity;
    pThreadPool->taskSize++;

    pthread_mutex_unlock(&pThreadPool->mutexPool);
}