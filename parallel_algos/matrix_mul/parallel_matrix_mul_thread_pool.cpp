#include <iostream>
#include <pthread.h>
#include "thread_pool.cpp"

using namespace std;

int **mat1, **mat2;

int** createmat(int n, int m, bool random = true)
{
    int** mat = new int*[n];
    for(int i = 0; i < n; i++)
        mat[i] = new int[m];
    for(int i = 0; i < n; i++)
        for(int j = 0; j < m; j++)
            mat[i][j] = random ? rand()%10 : 0;
    return mat;
}

void printmat(int** mat, int n, int m)
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

struct thread_arg
{
    static int m;
    int i, j;
    int* val;
    thread_arg()
    {
    }
    thread_arg(int i, int j, int* val) : i(i), j(j), val(val)
    {
    }
};
int thread_arg::m = 0;

void* multiply(void* void_arg)
{
    thread_arg arg = *(thread_arg*)void_arg;
    int m = arg.m;
    int i = arg.i;
    int j = arg.j;

    int ans = 0;
    for(int k = 0; k < m; k++)
        ans += mat1[i][k] * mat2[k][j];
    *(arg.val) = ans;
    return NULL;
}


int main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        cout << "Enter n, m, p";
        return -1;
    }

    srand(time(0));

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int p = atoi(argv[3]);

    thread_arg::m = m;

    mat1 = createmat(n, m);
    mat2 = createmat(m, p);
    int** ans = createmat(n, p, false);

    cout << "mat1\n";
    printmat(mat1, n, m);
    cout << "mat2\n";
    printmat(mat2, m, p);

    int max_threads = n*p;
    thread_arg thread_args[max_threads];
    int thread_count = 0;

    thread_pool* tpool = thread_pool_create(10);

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < p; j++)
        {
            thread_arg arg(i, j, &ans[i][j]);
            thread_args[thread_count] = arg;
            thread_pool_add_work(tpool, multiply, &thread_args[thread_count]);
            thread_count++;
        }
    }

    thread_pool_wait(tpool);
    thread_pool_destroy(tpool);

    cout << "ans\n";
    printmat(ans, n, p);
    
    return 0;
}