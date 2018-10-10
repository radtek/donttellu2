#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "DataPool.h"
#include "Common.h"
#include <semaphore.h>
#include "NetSocket.h"
//**** Define local
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static int g_TaskCounter = 0;
static pthread_t g_ptServer = 0;
static int g_nRun = 0;
static sem_t g_sem;
//****Public Function
void ReleaseDataPool(void)
{
    g_nRun = 0;
    sem_post(&g_sem);
    if(0 != g_ptServer)
    {
        void* retVal;
        pthread_join(g_ptServer,&retVal);
        g_ptServer = 0;
    }
    sem_destroy(&g_sem);
    g_TaskCounter = -100;
    pthread_mutex_destroy(&g_mutex);
}

static void DataPoolProc(void* lPvoid)
{
    int retVal = 0;
    printf("****DataPoolProc pthread_t=[%lx] Begin****\n",pthread_self());
    if(0 == sem_wait(&g_sem))
    {
        do
        {
            if(0 == pthread_mutex_lock(&g_mutex))
            {
                g_TaskCounter = 0;
                pthread_mutex_unlock(&g_mutex);
                ReplyAllClient();
            }
            if(sem_wait(&g_sem))
            {
                break;
            }
        }
        while(g_nRun);
    }
    printf("****DataPoolProc pthread_t=[%lx] End****\n",pthread_self());
    pthread_exit(&retVal);
}
int InitDataPool(void)
{
    sem_init (&g_sem, 0, 0);
    g_nRun = 1;
    int ret = pthread_create(&g_ptServer,NULL,(void*)(&DataPoolProc),NULL);
    printf("<%s(%d)%s>pthread_create=[%d]\n",__FILE__,__LINE__,__FUNCTION__,ret);
    return ret;
}
int AddTaskData(void)
{
    if(0 >= g_TaskCounter)
    {
        if(0 != pthread_mutex_lock(&g_mutex))
        {
            return -2;
        }
        ++g_TaskCounter;
        sem_post(&g_sem);
        pthread_mutex_unlock(&g_mutex);
    }
    return 0;
}


