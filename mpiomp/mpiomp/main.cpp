#include <chrono>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <string>
#include <chrono>
using namespace std;

double random_sqrt(double, int);
void sqrt_bench(int, int, int, int);

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	if (argc < 4) {
		printf("Arguments: threads iterations skip load");
		exit(1);
	}

	int t = 0;
	int i = 0;
	int s = 0;
	int l = 0;

	t = atoi(argv[1]);
	i = atoi(argv[2]);
	s = atoi(argv[3]);
	l = atoi(argv[4]);

	sqrt_bench(t, i, s, l);
	
	MPI_Finalize();
	return 0;
}

double random_sqrt(double v, int l)
{
	int i;
	for (i = 0; i < l*l; ++i)
	{
		v += rand()*rand();
		v = sqrt(v*v);
	}
	return v;
}

void sqrt_bench(int threadCount, int iterations, int skip, int load) {
	int i;
	double totalTime = 0.0;
	double* threadTimes = (double*)malloc(sizeof(double)*threadCount);
	
	for (i = 0; i < threadCount; ++i) {
		threadTimes[i] = 0.0;
	}

	int itr;
	double v = 0.0;
	for (itr = 0; itr < i; ++itr) {
		omp_set_num_threads(threadCount);

#pragma omp parallel
		{
			int num_t = omp_get_num_threads();
			int tid = omp_get_thread_num();

			auto t = std::chrono::system_clock::now();
			v = random_sqrt(v, load);
			if (itr < skip)
			{
				continue;
			}
			threadTimes[tid] += (std::chrono::system_clock::now() - t).count();
		}
	}

	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	for (i = 0; i < threadCount; ++i) {
		threadTimes[i] = threadTimes[i] / (iterations - skip);
	}

	double* timeDistribution = (double*)(malloc(sizeof(double)*size*threadCount));
	MPI_Gather(threadTimes, threadCount, MPI_DOUBLE, timeDistribution, threadCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (i = 0; i < size*threadCount; ++i) {
			cout << "rank: " + to_string((long long)(i / threadCount)) + " tid: " + to_string((long long)(i % threadCount)) + " time: " + to_string((long double)timeDistribution[i]) + "\n";
		}
	}

	free(timeDistribution);
	free(threadTimes);
	
}
