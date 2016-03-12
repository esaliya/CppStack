#include <iostream>
#include <omp.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

void simple(int n, float *a, float *b);

using namespace std;

int main() {
    cout << "Hello, World!" << endl;

    omp_set_num_threads(4);

#pragma omp parallel
    {
//        cout << to_string((long long)omp_get_num_threads()) + " " +  to_string((long long)omp_get_max_threads()) + " " + to_string((long long)omp_get_num_procs()) + " \n";
        cout << "Thread ID: " + to_string((long long) omp_get_thread_num()) + " Num Threads: " + to_string((long long)omp_get_num_threads()) + " " + to_string((long long)getpid()) + "\n";
        int i;
        int x = 100000000;
        double v = 0.0;
        for (i = 0; i < x*x; ++i)
        {
            v += rand()*rand();
            v = sqrt(v*v);
        }
        cout << v;
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
        cout << b[i] << '\n';
    }*/
    return 0;
}

void simple(int n, float *a, float *b)
{
    omp_set_num_threads(4);
    int i;
#pragma omp parallel
    {
        cout << "Thread ID: " + to_string((long long) omp_get_thread_num()) + " Num Threads: " + to_string((long long)omp_get_num_threads()) + to_string((long long)getpid()) + "\n";
#pragma omp for
        for (i = 1; i < n; i++) /* i is private by default */
        {

            b[i] = (a[i] + a[i - 1]) / 2.0;
        }
    }
}