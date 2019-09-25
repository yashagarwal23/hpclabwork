#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

#define PERMS 0666
#define SHARED_MEM_SIZE 4

#define NUM 10
key_t shmkey = 1234;
key_t semkey = 2345;

int semwait(int semid, int semn)
{
    struct sembuf sb[1];
    sb[0].sem_num = semn;
    sb[0].sem_op = -1; 
    sb[0].sem_flg = 0;
    semop(semid, sb, 1);
}

int semsignal(int semid, int semn)
{
    struct sembuf sb[1];
    sb[0].sem_num = semn;
    sb[0].sem_op = 1; 
    sb[0].sem_flg = 0;
    semop(semid, sb, 1);
}

int main(int argc, char *argv[])
{

    int semid = semget(semkey, 0, 0);
    int shmid = shmget(shmkey, SHARED_MEM_SIZE, 0666);
    int *shm = (int *)shmat(shmid, NULL, 0);
    printf("inside producer\n");
    for (int i = 1; i <= 10; i++)
    {
        semwait(semid, 0);
        int x = rand();
        printf("producer produced : %d\n", x);
        *shm = x;
        semsignal(semid, 1);
    }
    printf("exiting producer\n");
    return 0;
}