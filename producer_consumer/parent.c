#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

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
    semctl(semid, 0, SETVAL, arg1);
    arg2.val = 0;
    semctl(semid, 1, SETVAL, arg2);

    shmid = shmget(shmkey, SHARED_MEM_SIZE, PERMS);

    pid_t producer = fork();
    if(producer == 0)
    {
        char* argv[] = {"/usr/bin/gnome-terminal", "gnome-terminal", "--disable-factory", "--command=./producer", NULL};
        execv(argv[0], argv);
    }
    pid_t consumer = fork();
    if(consumer == 0)
    {
        char* argv[] = {"/usr/bin/gnome-terminal", "gnome-terminal", "--disable-factory", "--command=./consumer", NULL};
        execv(argv[0], argv);
    }
    waitpid(producer, NULL, 0);
    waitpid(consumer, NULL, 0);

    shmctl(shmid, 0, IPC_RMID);
    semctl(semid, 0, IPC_RMID);
    printf("parent exited\n");
}