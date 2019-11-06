#include <iostream>
#include <utility>
#include <numeric>
#include <pthread.h>
using namespace std;

typedef long long ll;

int n;
ll *arr;

struct sum_struct
{
    int startIndex;
    int endIndex;
    ll answer;
};

void* sum(void* arg)
{
    sum_struct* sum_arg = (sum_struct*)arg;
    int startIndex = sum_arg->startIndex;
    int endIndex = sum_arg->endIndex;

    if(endIndex - startIndex <= 10)
    {
        ll ans = 0;
        for(int i = startIndex; i <= endIndex; i++)
            ans += arr[i];
        sum_arg->answer = ans;
        return NULL;
    }

    int mid = (startIndex + endIndex)/2;
    sum_struct left_sum_arg;
    left_sum_arg.startIndex = startIndex;
    left_sum_arg.endIndex = mid;

    sum_struct right_sum_arg;
    right_sum_arg.startIndex = mid+1;
    right_sum_arg.endIndex = endIndex;

    pthread_t left, right;
    pthread_create(&left, NULL, sum, (void*)&left_sum_arg);
    pthread_create(&right, NULL, sum, (void*)&right_sum_arg);

    pthread_join(left, NULL);
    pthread_join(right, NULL);

    sum_arg->answer = left_sum_arg.answer + right_sum_arg.answer;
    return NULL;
}

// parallel sum each thread dividing task into 2 different threads

int main()
{
    cout << "Enter Number of elements : ";

    cin >> n;
    cout << "Enter array elements : ";
    arr = new ll[n];
    for (int i = 0; i < n; i++)
        cin >> arr[i];

    sum_struct arg;
    arg.startIndex = 0;
    arg.endIndex = n-1;
    
    sum((void*)&arg);

    cout << "sum : " << arg.answer << endl;

    return 0;
}