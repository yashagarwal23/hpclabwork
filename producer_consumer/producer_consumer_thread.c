#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h> 
#include <stdlib.h>

#define PROD_SEM_NAME "produced"
#define CONS_SEM_NAME "consumer"
#define PERMS 0666
#define NUM 10

int value = 10;
sem_t* prod_sem, *cons_sem;

void* producer_function()
{
    for(int i = 1; i <= NUM; i++)
    {
        sem_wait(prod_sem);
        int x = rand();
        printf("producer produced : %d \n", x);
        value = x;
        sem_post(cons_sem);
        sleep(1);
    }
}

void* consumer_function()
{
    for(int i = 1; i <= NUM; i++)
    {
        sem_wait(cons_sem);
        printf("consumer consumed : %d\n", value);
        sem_post(prod_sem);
        sleep(2);
    }
}

int main()
{
    prod_sem = sem_open(PROD_SEM_NAME, O_CREAT, PERMS, 0);
    sem_init(prod_sem, 0, 1);
    cons_sem = sem_open(CONS_SEM_NAME, O_CREAT, PERMS, 1);
    sem_init(cons_sem, 0, 0);

    pthread_t producer, consumer;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&producer, &attr, producer_function, NULL);
    pthread_create(&consumer, &attr, consumer_function, NULL);
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    sem_close(prod_sem);
    sem_close(cons_sem);

    return 0;
}