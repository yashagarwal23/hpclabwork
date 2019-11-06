#include <iostream>
#include <pthread.h>
using namespace std;
typedef long long ll;

int n;
ll *arr;
const int max_threads = 1000;

struct thread_arg
{
    int i, j;
    thread_arg()
    { 
    }
    thread_arg(int i, int j) : i(i), j(j)
    {    
    }
};

void* compute(void* _arg)
{
    thread_arg arg = *(thread_arg*)_arg;
    int i = arg.i;
    int j = arg.j;
    arr[i] += arr[j];
    pthread_exit(0);
}

class thread
{
    pthread_t pthread;
    bool Allocated;
    public:
        thread()
        {
            pthread = 0;
            Allocated = false; 
        }
        bool isAllocated()
        {
            return Allocated;
        }
        int create(void* func(void* ), void* arg)
        {
            Allocated = true;
            return pthread_create(&pthread, NULL, func, arg);
        }
        int join()
        {
            Allocated = false;
            return pthread_join(pthread, NULL);
        }
};

int main()
{
    cout << "Enter Number of elements : ";

    cin >> n;
    cout << "Enter array elements : ";
    arr = new ll[n];
    for (int i = 0; i < n; i++)
        cin >> arr[i];

    thread threads[max_threads];
    thread_arg args[max_threads];

    int num = n;
    while(num != 1)
    {
        for(int i = 0; i < num/2; i++)
        {
            args[i%max_threads] = thread_arg(i, num-i-1);
            if(!threads[i%max_threads].isAllocated())
                threads[i%max_threads].join();
            threads[i%max_threads].create(compute, (void*)&args[i%max_threads]);
        }
        for(int i = 0; i < num/2; i++)
            threads[i%max_threads].join();
        
        num = num - num/2;
    }
    cout << "sum : " << arr[0] << endl;
    return 0;
}