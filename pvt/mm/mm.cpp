#include <iostream>
#include<mpi.h>
#include<omp.h>
#include<cstdlib>

using namespace std;
void block_matrix_multiply(double* A, double* B, int a_rows, int b_cols, int ab_commn, int block_size, double* C);

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	if (argc < 7) {
		printf("Arguments: thread_count iterations a_rows b_cols ab_commn block_size");
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

	thread_count = atoi(argv[1]);
	iterations = atoi(argv[2]);
	a_rows = atoi(argv[3]);
	b_cols = atoi(argv[4]);
	ab_comn = atoi(argv[5]);
	block_size = atoi(argv[6]);

	omp_set_num_threads(thread_count);
#pragma omp parallel
	{
		int num_threads = omp_get_num_threads();
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

		block_matrix_multiply(A, B, a_rows, b_cols, ab_comn, block_size, C);

	}
	MPI_Finalize();
    return 0;
}

void block_matrix_multiply(
	double* A, double* B, int a_rows, int b_cols, int ab_commn, int block_size, double* C)
{
	int a_height_blocks = a_rows / block_size; // size = Height of A
	int a_last_block_height = a_rows - (a_height_blocks * block_size);
	if (a_last_block_height > 0)
	{
		a_height_blocks++;
	}

	int b_width_blocks = b_cols / block_size; // size = Width of B
	int b_last_block_width = b_cols - (b_width_blocks * block_size);
	if (b_last_block_width > 0)
	{
		b_width_blocks++;
	}

	int commn_blocks = ab_commn / block_size; // size = Width of A or Height of B
	int commLastBlockWidth = ab_commn - (commn_blocks * block_size);
	if (commLastBlockWidth > 0)
	{
		commn_blocks++;
	}

	int a_block_height = block_size;
	int b_block_width;
	int comm_block_width;

	double k_tmp;
	int i_offset;
	for (int ib = 0; ib < a_height_blocks; ib++)
	{
		if (a_last_block_height > 0 && ib == (a_height_blocks - 1))
		{
			a_block_height = a_last_block_height;
		}
		b_block_width = comm_block_width = block_size;
		for (int jb = 0; jb < b_width_blocks; jb++)
		{
			if (b_last_block_width > 0 && jb == (b_width_blocks - 1))
			{
				b_block_width = b_last_block_width;
			}
			comm_block_width = block_size;
			for (int kb = 0; kb < commn_blocks; kb++)
			{
				if (commLastBlockWidth > 0 && kb == (commn_blocks - 1))
				{
					comm_block_width = commLastBlockWidth;
				}

				for (int i = ib * block_size; i < (ib * block_size) + a_block_height; i++)
				{
					i_offset = i * b_cols;
					for (int j = jb * block_size; j < (jb * block_size) + b_block_width;
					     j++)
					{
						for (int k = kb * block_size;
						     k < (kb * block_size) + comm_block_width; k++)
						{
							k_tmp = B[k * b_cols + j];
							if (A[i*ab_commn+k] != 0 && k_tmp != 0)
							{
								C[i_offset + j] += A[i*ab_commn + k] * k_tmp;
							}
						}
					}
				}
			}
		}
	}
}
