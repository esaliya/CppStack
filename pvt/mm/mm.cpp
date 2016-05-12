#include "mm.h"
#include <iostream>
#include<mpi.h>
#include<omp.h>
#include<cstdlib>
#include <chrono>
#include <ctime>
#include <vector>

using namespace std;
using namespace std::chrono;

void mm_lrt_global(int rank, int thread_count, int iterations, int a_rows, int b_cols, int ab_comn, int block_size)
{
	double* A = static_cast<double*>(malloc(sizeof(double)*a_rows*ab_comn*thread_count));
	double* B = static_cast<double*>(malloc(sizeof(double)*b_cols*ab_comn*thread_count));
	double* C = static_cast<double*>(malloc(sizeof(double)*a_rows*b_cols*thread_count));


	for (int i = 0; i < a_rows*ab_comn*thread_count; ++i)
	{
		A[i] = ((i & 1) == 0) ? (0.9999995 / 1.0000023) : (1.0000023 / 0.9999995);
	}
	for (int i = 0; i < b_cols*ab_comn*thread_count; ++i)
	{
		B[i] = ((i & 1) == 0) ? (1.0000023 / 0.9999995) : (0.9999995 / 1.0000023);
	}
	for (int i = 0; i < a_rows*b_cols*thread_count; ++i)
	{
		C[i] = 0.0;
	}

	omp_set_num_threads(thread_count);
#pragma omp parallel
	{
		int num_threads = omp_get_num_threads();
		int thread_id = omp_get_thread_num();
		if (thread_count != num_threads)
		{
			cout << "Thread count " << thread_count << " mismatch with omp thread count " << num_threads << "\n";
		}


		time_point<system_clock> start, end;
		start = system_clock::now();
		for (int itr = 0; itr < iterations; ++itr)
		{
			block_matrix_multiply_with_thread_offset(A, B, a_rows, b_cols, ab_comn, block_size, C, thread_id);
		}
		end = system_clock::now();

		duration<double> elapsed_seconds = end - start;
		time_t end_time = system_clock::to_time_t(end);

		cout << "rank " << rank << " thread " << thread_id << " elapsed " << elapsed_seconds.count() << "s\n";
	}

	free(A);
	free(B);
	free(C);
}

void mm_lrt_local(int rank, int thread_count, int iterations, int a_rows, int b_cols, int ab_comn, int block_size)
{
	omp_set_num_threads(thread_count);
#pragma omp parallel
	{
		int num_threads = omp_get_num_threads();
		int thread_id = omp_get_thread_num();
		if (thread_count != num_threads)
		{
			cout << "Thread count " << thread_count << " mismatch with omp thread count " << num_threads << "\n";
		}

		double* A = static_cast<double*>(malloc(sizeof(double)*a_rows*ab_comn));
		double* B = static_cast<double*>(malloc(sizeof(double)*b_cols*ab_comn));
		double* C = static_cast<double*>(malloc(sizeof(double)*a_rows*b_cols));


		for (int i = 0; i < a_rows*ab_comn; ++i)
		{
			A[i] = ((i & 1) == 0) ? (0.9999995 / 1.0000023) : (1.0000023 / 0.9999995);
		}
		for (int i = 0; i < b_cols*ab_comn; ++i)
		{
			B[i] = ((i & 1) == 0) ? (1.0000023 / 0.9999995) : (0.9999995 / 1.0000023);
		}
		for (int i = 0; i < a_rows*b_cols; ++i)
		{
			C[i] = 0.0;
		}
		time_point<system_clock> start, end;
		start = system_clock::now();
		for (int itr = 0; itr < iterations; ++itr)
		{
			block_matrix_multiply(A, B, a_rows, b_cols, ab_comn, block_size, C);
		}
		end = system_clock::now();

		duration<double> elapsed_seconds = end - start;
		time_t end_time = system_clock::to_time_t(end);

		cout << "rank " << rank << " thread " << thread_id << " elapsed " << elapsed_seconds.count() << "s\n";

		free(A);
		free(B);
		free(C);

	}
}

void mm_fj_global(int rank, int thread_count, int iterations, int a_rows, int b_cols, int ab_comn, int block_size)
{
	omp_set_num_threads(thread_count);
	vector<double> times(thread_count);
	for (int i = 0; i < thread_count; ++i)
	{
		times[i] = 0.0;
	}

	double* A = static_cast<double*>(malloc(sizeof(double)*a_rows*ab_comn*thread_count));
	double* B = static_cast<double*>(malloc(sizeof(double)*b_cols*ab_comn*thread_count));
	double* C = static_cast<double*>(malloc(sizeof(double)*a_rows*b_cols*thread_count));


	for (int i = 0; i < a_rows*ab_comn*thread_count; ++i)
	{
		A[i] = ((i & 1) == 0) ? (0.9999995 / 1.0000023) : (1.0000023 / 0.9999995);
	}
	for (int i = 0; i < b_cols*ab_comn*thread_count; ++i)
	{
		B[i] = ((i & 1) == 0) ? (1.0000023 / 0.9999995) : (0.9999995 / 1.0000023);
	}
	for (int i = 0; i < a_rows*b_cols*thread_count; ++i)
	{
		C[i] = 0.0;
	}

	for (int itr = 0; itr < iterations; ++itr)
	{
#pragma omp parallel
	{
		int num_threads = omp_get_num_threads();
		int thread_id = omp_get_thread_num();
		if (thread_count != num_threads)
		{
			cout << "Thread count " << thread_count << " mismatch with omp thread count " << num_threads << "\n";
		}


#pragma omp barrier
		time_point<system_clock> start, end;
		start = system_clock::now();
		block_matrix_multiply_with_thread_offset(A, B, a_rows, b_cols, ab_comn, block_size, C, thread_id);
		end = system_clock::now();
		duration<double> elapsed_seconds = end - start;
#pragma omp barrier
		times[thread_id] += elapsed_seconds.count();
	}
	}

	free(A);
	free(B);
	free(C);

	for (int i = 0; i < thread_count; ++i)
	{
		cout << "rank " << rank << " thread " << i << " elapsed " << times[i] << "s\n";
	}
}

void mm_fj_local(int rank, int thread_count, int iterations, int a_rows, int b_cols, int ab_comn, int block_size)
{
	omp_set_num_threads(thread_count);
	vector<double> times(thread_count);
	for (int i = 0; i < thread_count; ++i)
	{
		times[i] = 0.0;
	}

	for (int itr = 0; itr < iterations; ++itr)
	{
#pragma omp parallel
	{
		int num_threads = omp_get_num_threads();
		int thread_id = omp_get_thread_num();
		if (thread_count != num_threads)
		{
			cout << "Thread count " << thread_count << " mismatch with omp thread count " << num_threads << "\n";
		}

		double* A = static_cast<double*>(malloc(sizeof(double) * a_rows * ab_comn));
		double* B = static_cast<double*>(malloc(sizeof(double) * b_cols * ab_comn));
		double* C = static_cast<double*>(malloc(sizeof(double) * a_rows * b_cols));


		for (int i = 0; i < a_rows * ab_comn; ++i)
		{
			A[i] = ((i & 1) == 0) ? (0.9999995 / 1.0000023) : (1.0000023 / 0.9999995);
		}
		for (int i = 0; i < b_cols * ab_comn; ++i)
		{
			B[i] = ((i & 1) == 0) ? (1.0000023 / 0.9999995) : (0.9999995 / 1.0000023);
		}
		for (int i = 0; i < a_rows * b_cols; ++i)
		{
			C[i] = 0.0;
		}
#pragma omp barrier
		time_point<system_clock> start, end;
		start = system_clock::now();
		block_matrix_multiply(A, B, a_rows, b_cols, ab_comn, block_size, C);
		end = system_clock::now();
		duration<double> elapsed_seconds = end - start;
#pragma omp barrier
		times[thread_id] += elapsed_seconds.count();


		free(A);
		free(B);
		free(C);
	}
	}
	for (int i = 0; i < thread_count; ++i)
	{
		cout << "rank " << rank << " thread " << i << " elapsed " << times[i] << "s\n";
	}
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	if (argc < 9) {
		printf("Arguments: thread_count iterations a_rows b_cols ab_commn block_size lrt local");
		exit(1);
	}

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Barrier(MPI_COMM_WORLD);

	int thread_count = 0;
	int iterations = 0;
	int a_rows = 0;
	int b_cols = 0;
	int ab_comn = 0;
	int block_size = 0;
	int lrt = 1;
	int local = 1;

	thread_count = atoi(argv[1]);
	iterations = atoi(argv[2]);
	a_rows = atoi(argv[3]);
	b_cols = atoi(argv[4]);
	ab_comn = atoi(argv[5]);
	block_size = atoi(argv[6]);
	lrt = atoi(argv[7]);
	local = atoi(argv[8]);

	if (lrt == 1 && local == 1)
	{
		mm_lrt_local(rank, thread_count, iterations, a_rows, b_cols, ab_comn, block_size);
	}
	else if (lrt == 0 && local == 1)
	{
		mm_fj_local(rank, thread_count, iterations, a_rows, b_cols, ab_comn, block_size);
	}
	else if (lrt == 1 && local == 0)
	{
		mm_lrt_global(rank, thread_count, iterations, a_rows, b_cols, ab_comn, block_size);
	}
	else if (lrt == 0 && local == 0)
	{
		mm_fj_global(rank, thread_count, iterations, a_rows, b_cols, ab_comn, block_size);
	}
	MPI_Finalize();
	return 0;
}