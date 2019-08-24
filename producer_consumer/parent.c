#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>

#define PERMS (IPC_CREAT | 0666)
#define SHARED_MEM_SIZE 4

key_t shmkey = 1234;
int shmid;

key_t semkey = 2345;
int semid;

union semun {
    int val;
    struct semid_ds *buf;
    short *array;
} arg1, arg2;

int main()
{
    semid = semget(semkey, 2, PERMS);
    arg1.val = 1;
    int i = semctl(semid, 0, SETVAL, arg1);
    // printf("%d", i);
    arg2.val = 0;
    i = semctl(semid, 1, SETVAL, arg2);
    // printf("%d", i);
    shmid = shmget(shmkey, SHARED_MEM_SIZE, IPC_CREAT | 0666);
    pid_t producer = fork();
    if (producer == 0)
    {
        char *arg[] = {"./producer", NULL};
        execv(arg[0], arg);
    }

    pid_t consumer = fork();
    if (consumer == 0)
    {
        char *arg[] = {"./consumer", NULL};
        execv(arg[0], arg);
    }
    printf("back to main\n");
    waitpid(producer, NULL, 0);
    waitpid(consumer, NULL, 0);

    // wait(NULL);
    // wait(NULL);

    shmctl(shmid, 0, IPC_RMID);
    semctl(semid, 0, IPC_RMID);
    printf("parent exited\n");
}