#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
using namespace std;

key_t shmkey = 1234;

int main()
{
    cout << "Enter number of elements : ";
    int n;
    cin >> n;

    int shmid = shmget(shmkey, n * sizeof(int), IPC_CREAT | 0666);
    int *arr = (int *)shmat(shmid, NULL, 0);

    cout << "Enter array elements : ";
    for (int i = 0; i < n; i++)
        cin >> arr[i];

    int max_process = (n + 1) / 2;
    pid_t processes[max_process];

    for (int step = 1; step <= n; step++)
    {
        int start = step % 2 ? 1 : 0;
        for (int i = 0; i < max_process; i++)
        {
            if ((processes[i] = fork()) == 0)
            {
                int index = start + 2 * i;

                int shmid = shmget(shmkey, n * sizeof(int), 0666);
                int *arr = (int *)shmat(shmid, NULL, 0);

                if (index + 1 < n && arr[index] > arr[index + 1])
                    swap(arr[index], arr[index + 1]);
                exit(0);
            }
        }
        for (int i = 0; i < max_process; i++)
            waitpid(processes[i], NULL, 0);
    }

    cout << "sorted array : ";
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << endl;

    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}