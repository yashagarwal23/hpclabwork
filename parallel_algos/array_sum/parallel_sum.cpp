#include <iostream>
#include <pthread.h>
using namespace std;
typedef long long ll;

int n;
ll *arr;

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

int main()
{
    cout << "Enter Number of elements : ";

    cin >> n;
    cout << "Enter array elements : ";
    arr = new ll[n];
    for (int i = 0; i < n; i++)
        cin >> arr[i];

    int max_threads = n/2;
    pthread_t threads[max_threads];
    thread_arg args[max_threads];

    int num = n;
    while(num != 1)
    {
        for(int i = 0; i < num/2; i++)
        {
            args[i] = thread_arg(i, num-i-1);
            pthread_create(&threads[i], NULL, compute, (void*)&args[i]);
        }
        for(int i = 0; i < num/2; i++)
            pthread_join(threads[i], NULL);
        
        num = num - num/2;
    }
    cout << "sum : " << arr[0] << endl;
    return 0;
}