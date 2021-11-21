#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "thread_pool.h"

void func(void *arg)
{
    int num = 0;

    num = *(int*)arg;
    printf("create thread: %ld, work func num:%d\n", pthread_self(), num);
    sleep(1);
}

int main()
{
    int num = 100;
    ThreadPool *pThreadPool = NULL;

    threadPoolCreate(pThreadPool, 10, 30, 100);
    for (int i = 0; i < 100; ++i)
    {
        num++;
        threadPoolAddTask(pThreadPool, func, (void*)&num);
    }
}