#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "thread_pool.h"

void taskFunc(void* arg)
{
    int num = *(int*)arg;
    printf("thread %ld is working, number = %d\n",
        pthread_self(), num);
    sleep(1);
}

int main()
{
    int ret = 0;
    ThreadPool *pThreadPool = NULL;

    ret = threadPoolCreate(pThreadPool, 3, 10, 100);
    if (ret == 0)
    {
        printf("threadPoolCreate success.\n");
    }

    for (int i = 0; i < 100; ++i)
    {
        int* num = (int*)malloc(sizeof(int));
        *num = i + 100;
        threadPoolAddTask(pThreadPool, taskFunc, num);
    }

    sleep(10);

    ret = threadPoolDestroy(pThreadPool);
    if (ret == 0)
    {
        printf("threadPoolDestroy success.\n");
    }
    return 0;
}