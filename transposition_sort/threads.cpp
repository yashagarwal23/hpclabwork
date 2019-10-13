#include <iostream>
#include <pthread.h>
using namespace std;

int n;
int *arr;

void *compare(void *arg)
{
    int index = *(int *)arg;
    if (index + 1 < n && arr[index] > arr[index + 1])
    {
        swap(arr[index], arr[index + 1]);
    }
}

int main()
{
    cout << "Enter number of elements : ";
    cin >> n;
    cout << "Enter array elements : ";
    arr = new int[n];
    for (int i = 0; i < n; i++)
        cin >> arr[i];

    int max_threads = (n + 1) / 2;
    pthread_t threads[max_threads];
    int comapre_index[max_threads];

    for (int step = 1; step <= n; step++)
    {
        int start = step % 2 ? 1 : 0;
        for (int i = 0; i < max_threads; i++)
        {
            comapre_index[i] = start + 2 * i;
            pthread_create(&threads[i], NULL, compare, (void *)&comapre_index[i]);
        }
        for (int i = 0; i < max_threads; i++)
            pthread_join(threads[i], NULL);
    }

    cout << "sorted array : ";
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << endl;
    return 0;
}