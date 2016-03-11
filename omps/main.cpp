#include <iostream>
#include <omp.h>
#include <string>

void simple(int n, float *a, float *b);

using namespace std;

int main() {
    cout << "Hello, World!" << endl;

#pragma omp parallel
    {
        omp_set_num_threads(4);
        cout << to_string((long long)omp_get_num_threads()) + " " +  to_string((long long)omp_get_max_threads()) + " " + to_string((long long)omp_get_num_procs()) + " \n";
    }

/*
    float a[8];
    float b[8];
    int i = 0;
    for (i = 0; i < 8; ++i){
        a[i] = i;
    }
    simple(8, a, b);
    for (i = 0; i < 8; ++i){
        //cout << b[i] << '\n';
    }*/
    return 0;
}

void simple(int n, float *a, float *b)
{
    int i;
#pragma omp parallel
    {
        omp_set_num_threads(2);
        cout << "Thread ID: " + to_string((long long) omp_get_thread_num()) + " Num Threads: " + to_string((long long)omp_get_num_threads()) + "\n";
#pragma omp for
        for (i = 1; i < n; i++) /* i is private by default */
        {

            b[i] = (a[i] + a[i - 1]) / 2.0;
        }
    }
}