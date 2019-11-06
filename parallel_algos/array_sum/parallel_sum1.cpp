#include <iostream>
#include <pthread.h>
using namespace std;
typedef long long ll;

int n;
ll *arr;

pthread_mutex_t sum_mutex;
ll answer;

const int NUM_THREADS = 10;

void* sum(void* arg)
{
    int iterations = n/NUM_THREADS;
    int k = (*(int*)arg);
    int start = k*iterations;
    int end = (k == NUM_THREADS -1 ? n : start + iterations);

    ll temp_sum = 0;
    for(int i = start; i < end; i++)
        temp_sum += arr[i];

    pthread_mutex_lock(&sum_mutex);
    answer += temp_sum;
    pthread_mutex_unlock(&sum_mutex);
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

    pthread_mutex_init(&sum_mutex, NULL);

    pthread_t threads[NUM_THREADS];
    int tids[NUM_THREADS];
    for(int i = 0; i < NUM_THREADS; i++)
    {
        tids[i] = i;
        pthread_create(&threads[i], NULL, sum, (void*)&tids[i]);
    }
    for(int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    cout << "sum : " << answer << endl;
    
    pthread_mutex_destroy(&sum_mutex);

    return 0;
}