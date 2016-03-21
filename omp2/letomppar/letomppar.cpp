// letomppar.cpp : Defines the entry point for the console application.
//

#include <chrono>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <string>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>

using namespace std;
using namespace std::chrono;

void matrixMultiply(double*, double*, int, int, int, int, double*, int, int);
void bcReplica(int, int, int, int);

double* threadPartialBofZ;
double* preX;
double* threadPartialOutMM;
int targetDimension = 3;
int blockSize = 64;

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	if (argc < 5) {
		printf("Arguments: threadCount iterations rowCountPerUnit colCount");
		exit(1);
	}

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

/*	if (rank == 0) 
	{
		do {
       			cout << '\n' << "Rank 0 PID " << getpid()  <<"\nPress the Enter key to continue.";
		   } while (cin.get() != '\n');
	}*/

	MPI_Barrier(MPI_COMM_WORLD);

	int t = 0;
	int i = 0;
	int c = 0;
	int r = 0;

	t = atoi(argv[1]);
	i = atoi(argv[2]);
	r = atoi(argv[3]);
	c = atoi(argv[4]);

	bcReplica(t, i, c, r);

	MPI_Finalize();
	return 0;
}

void matrixMultiply(double* A, double* B, int aHeight, int bWidth, int comm, int bz, double* C, int threadAOffset, int threadCOffset) {

	int aHeightBlocks = aHeight / bz; // size = Height of A
	int aLastBlockHeight = aHeight - (aHeightBlocks * bz);
	if (aLastBlockHeight > 0) {
		aHeightBlocks++;
	}

	int bWidthBlocks = bWidth / bz; // size = Width of B
	int bLastBlockWidth = bWidth - (bWidthBlocks * bz);
	if (bLastBlockWidth > 0) {
		bWidthBlocks++;
	}

	int commnBlocks = comm / bz; // size = Width of A or Height of B
	int commLastBlockWidth = comm - (commnBlocks * bz);
	if (commLastBlockWidth > 0) {
		commnBlocks++;
	}

	int aBlockHeight = bz;
	int bBlockWidth;
	int commBlockWidth;

	int ib, jb, kb, i, j, k;
	int iARowOffset, kBRowOffset, iCRowOffset;
	for (ib = 0; ib < aHeightBlocks; ib++) {
		if (aLastBlockHeight > 0 && ib == (aHeightBlocks - 1)) {
			aBlockHeight = aLastBlockHeight;
		}
		bBlockWidth = bz;
		for (jb = 0; jb < bWidthBlocks; jb++) {
			if (bLastBlockWidth > 0 && jb == (bWidthBlocks - 1)) {
				bBlockWidth = bLastBlockWidth;
			}
			commBlockWidth = bz;
			for (kb = 0; kb < commnBlocks; kb++) {
				if (commLastBlockWidth > 0 && kb == (commnBlocks - 1)) {
					commBlockWidth = commLastBlockWidth;
				}

				for (i = ib * bz; i < (ib * bz) + aBlockHeight; i++) {
					iARowOffset = i*comm + threadAOffset;
					iCRowOffset = i*bWidth + threadCOffset;
					for (j = jb * bz; j < (jb * bz) + bBlockWidth;
					j++) {
						for (k = kb * bz;
						k < (kb * bz) + commBlockWidth; k++) {
							kBRowOffset = k*bWidth;
							if (A[iARowOffset + k] != 0 && B[kBRowOffset + j] != 0) {
								C[iCRowOffset + j] += A[iARowOffset + k] * B[kBRowOffset + j];
							}
						}
					}
				}
			}
		}
	}
}

void bcReplica(int threadCount, int iterations, int globalColCount, int rowCountPerUnit) {
	omp_set_num_threads(threadCount);

#pragma omp parallel
	{
		int pointComponentCountGlobal = globalColCount * targetDimension;
		int pointComponentCountLocal = rowCountPerUnit * targetDimension;
		preX = (double*)malloc(sizeof(double) * pointComponentCountGlobal);
		int i;

		int pairCountLocal = rowCountPerUnit * globalColCount;
		threadPartialBofZ = (double*)malloc(sizeof(double) * 1 * pairCountLocal);
		threadPartialOutMM = (double*)malloc(sizeof(double*) * 1 * pointComponentCountLocal);
		int j;


		double totalTime = 0.0;

		int itr;
		int k;

		double* threadTimes = (double*)malloc(sizeof(double)*1);
		for (i = 0; i < 1; ++i) {
			threadTimes[i] = 0.0;
		}

		double v = 0.0;
		for (i = 0; i < pointComponentCountGlobal; ++i) {
			preX[i] = (double)rand() / (double)RAND_MAX;
		}

		int pairCountAllThreads = 1*pairCountLocal;
		for (k = 0; k < pairCountAllThreads; ++k) {
			threadPartialBofZ[k] = (double)rand() / (double)RAND_MAX;
		}

		int pointComponentCountAllThreads = 1*pointComponentCountLocal;
		for (k = 0; k < pointComponentCountAllThreads; ++k) {
			threadPartialOutMM[k] = (double)0.0;
		}



		for (itr = 0; itr < iterations; ++itr) {
			int num_t = omp_get_num_threads();
			int tid = omp_get_thread_num();

			//auto t = high_resolution_clock::now();
			matrixMultiply(threadPartialBofZ, preX, rowCountPerUnit, targetDimension, globalColCount, blockSize, threadPartialOutMM, 0*pairCountLocal, 0*pointComponentCountLocal);
			//threadTimes[tid] += duration_cast<milliseconds>(high_resolution_clock::now() - t).count();
		}

		/*int rank;
		int size;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		for (i = 0; i < threadCount; ++i)
		{
			threadTimes[i] = threadTimes[i] / iterations;
		}

		double* timeDistribution = (double*)(malloc(sizeof(double)*size*threadCount));
		MPI_Gather(threadTimes, threadCount, MPI_DOUBLE, timeDistribution, threadCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		if (rank == 0) {
			for (i = 0; i < size*threadCount; ++i) {
				cout << "rank: " + to_string((long long)(i / threadCount)) + " tid: " + to_string((long long)(i % threadCount)) + " time: " + to_string((long double)timeDistribution[i]) + "\n";
			}
		}*/
	}
}
