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

int semwait(int semid, int semn)
{
    // printf("vdvjbfjv\n");
    struct sembuf sb[1];
    sb[0].sem_num = semn;
    sb[0].sem_op = -1; 
    sb[0].sem_flg = 0;
    semop(semid, sb, 1);
    // printf("dnvfjvn\n");
}

int semsignal(int semid, int semn)
{
    struct sembuf sb[1];
    sb[0].sem_num = semn;
    sb[0].sem_op = 1; 
    sb[0].sem_flg = 0;
    semop(semid, sb, 1);
}


int main()
{
    semid = semget(semkey, 2, PERMS);
    // printf("%d\n", semid);
    arg1.val = 1;
    int i = semctl(semid, 0, SETVAL, arg1);
    // printf("%d", i);
    arg2.val = 0;
    i = semctl(semid, 1, SETVAL, arg2);
    // printf("%d", i);

    // i = semctl(semid, 0, GETVAL);
    // printf("%d\n", i);
    // i = semctl(semid, 1, GETVAL);
    // printf("%d\n", i);

    shmid = shmget(shmkey, SHARED_MEM_SIZE, IPC_CREAT | 0666);
    pid_t producer = fork();
    if (producer == 0)
    {
        // system("gnome-terminal -e \"./producer\"");
        // exit(0);
        char *arg[] = {"./producer", "producer",  NULL};
        execv(arg[0], arg);
    }

    pid_t consumer = fork();
    if (consumer == 0)
    {
        // system("gnome-terminal -e \"./consumer\"");
        // exit(0);
        char *arg[] = {"./consumer", "consumer", NULL};
        execv(arg[0], arg);
    }
    waitpid(producer, NULL, 0);
    waitpid(consumer, NULL, 0);

    shmctl(shmid, 0, IPC_RMID);
    semctl(semid, 0, IPC_RMID);
    printf("parent exited\n");
}