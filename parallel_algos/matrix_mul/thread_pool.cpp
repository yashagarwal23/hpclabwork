#ifndef __tmOOL_SINGLE_H__
#define __tmOOL_SINGLE_H__

#include <pthread.h>
#include <stdlib.h>

typedef void* (*thread_func_t)(void *arg);

struct thread_pool_work {
    thread_func_t      func;
    void              *arg;
    struct thread_pool_work *next;
};

struct thread_pool {
    thread_pool_work    *work_first;
    thread_pool_work    *work_last;
    pthread_mutex_t  work_mutex;
    pthread_cond_t   work_cond;
    pthread_cond_t   working_cond;
    size_t           working_cnt;
    size_t           thread_cnt;
    bool             stop;
};

static thread_pool_work* thread_pool_work_create(thread_func_t func, void* arg)
{
    thread_pool_work* work;
    if(func == NULL)
        return NULL;
    
    work = (thread_pool_work*) malloc(sizeof(thread_pool_work));
    work->func = func;
    work->arg = arg;
    work->next = NULL;
    return work;
}

static void thread_pool_work_destroy(thread_pool_work* work)
{
    if(work == NULL)
        return;
    free(work);
}


static thread_pool_work* thread_pool_get_work(thread_pool* tm)
{
    if(tm == NULL)
        return NULL;
    
    thread_pool_work* work = tm->work_first;
    if(work == NULL)
        return NULL;
    
    if(work->next = NULL)
    {
        tm->work_first = NULL;
        tm->work_last = NULL;
    }
    else
    {
        tm->work_first = work->next;
    }
    return work;
}

void* thread_pool_worker(void* arg)
{
    thread_pool *tm = (thread_pool*)arg;
    thread_pool_work* work;

    while(1)
    {
        pthread_mutex_lock(&(tm->work_mutex));
        if(tm->stop)
            break;
        
        if(tm->work_first == NULL)
            pthread_cond_wait(&(tm->work_cond), &(tm->work_mutex));
        
        work = thread_pool_get_work(tm);
        tm->working_cnt++;
        pthread_mutex_unlock(&tm->work_mutex);

        if(work != NULL)
        {
            work->func(work->arg);
            thread_pool_work_destroy(work);
        }

        pthread_mutex_lock(&tm->work_mutex);
        tm->working_cnt--;
        if(!tm->stop && tm->working_cnt == 0 && tm->work_first == NULL)
            pthread_cond_signal(&tm->working_cond);
        pthread_mutex_unlock(&tm->work_mutex);
    }
    tm->thread_cnt--;
    pthread_cond_signal(&tm->working_cond);
    pthread_mutex_unlock(&tm->work_mutex);
    return NULL;
}

thread_pool* thread_pool_create(int num=5)
{
    thread_pool* tm;
    pthread_t thread;

    if(num == 0)
        num = 5;

    tm = (thread_pool*)calloc(1, sizeof(*tm));
    tm->thread_cnt = num;
    pthread_mutex_init(&tm->work_mutex, NULL);
    pthread_cond_init(&tm->working_cond, NULL);
    pthread_cond_init(&tm->work_cond, NULL);

    tm->work_first = NULL;
    tm->work_last = NULL;
    tm->stop = false;

    for(int i = 0; i < num; i++)
    {
        pthread_create(&thread, NULL, thread_pool_worker, tm);
        pthread_detach(thread);
    }

    return tm;
}

bool thread_pool_add_work(thread_pool* tm, thread_func_t func, void* arg)
{
    if(tm == NULL)
        return false;
    thread_pool_work* work = thread_pool_work_create(func, arg);
    if(work == NULL)
        return false;
    
    pthread_mutex_lock(&tm->work_mutex);
    if(tm->work_first == NULL)
    {
        tm->work_first = work;
        tm->work_last = tm->work_first;
    }
    else
    {
        tm->work_last->next = work;
        tm->work_last = work;
    }
    pthread_mutex_unlock(&(tm->work_mutex));
    pthread_cond_broadcast(&(tm->work_cond));

    return true;
}

void thread_pool_wait(thread_pool *tm)
{
    if (tm == NULL)
        return;

    pthread_mutex_lock(&(tm->work_mutex));
    while (1) {
        if ((!tm->stop && tm->working_cnt != 0) || (tm->stop && tm->thread_cnt != 0)) {
            pthread_cond_wait(&(tm->working_cond), &(tm->work_mutex));
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(tm->work_mutex));
}

void thread_pool_destroy(thread_pool* tm)
{
    thread_pool_work* work;
    thread_pool_work* work2;

    if(tm == NULL)
        return;
    pthread_mutex_lock(&tm->work_mutex);
    work = tm->work_first;
    while(work != NULL)
    {
        work2 = work->next;
        thread_pool_work_destroy(work);
        work = work2;
    }
    tm->stop = true;
    pthread_cond_broadcast(&tm->work_cond);
    pthread_mutex_unlock(&tm->work_mutex);

    thread_pool_wait(tm);

    pthread_mutex_destroy(&tm->work_mutex);
    pthread_cond_destroy(&(tm->work_cond));
    pthread_cond_destroy(&(tm->working_cond));

    free(tm);
}

#endif