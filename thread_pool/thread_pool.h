#ifndef __TPOOL_H__
#define __TPOOL_H__

#include <pthread.h>

typedef void (*thread_func_t)(void *arg);

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

thread_pool* thread_pool_create(size_t num);
void thread_pool_destroy(thread_pool* tm);

bool thread_pool_add_work(thread_pool* tm, thread_func_t func, void* arg);
void thread_pool_wait(thread_pool* tm);

#endif