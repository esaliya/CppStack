#include <iostream>
#include <chrono>
#include <assert.h>
#include "mpi.h"
#define CHUNK_SIZE 134217728

typedef std::chrono::duration<double, std::milli> ms_t;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> ticks_t;
typedef std::chrono::high_resolution_clock hrc_t;

void print_timing(const std::chrono::time_point<std::chrono::high_resolution_clock> &start_ms,
                  const std::chrono::time_point<std::chrono::high_resolution_clock> &end_ms, const std::string &msg);
void MPI_Bcast_chunk(int* arr, unsigned long length, int root, int rank);

int world_proc_rank;
int world_procs_count;

int main(int argc, char *argv[]) {
  int i, rank, size, len;
  char name[MPI_MAX_PROCESSOR_NAME];
  MPI::Status stat;

  MPI::Init(argc, argv);

  size = MPI::COMM_WORLD.Get_size();
  world_procs_count = size;
  rank = MPI::COMM_WORLD.Get_rank();
  world_proc_rank = rank;
  /*MPI::Get_processor_name(name, len);

  if (rank == 0) {

    std::cout << "Hello world: rank " << rank << " of " << size << " running on " << name << "\n";

    for (i = 1; i < size; i++) {
      MPI::COMM_WORLD.Recv(&rank, 1, MPI_INT, i, 1, stat);
      MPI::COMM_WORLD.Recv(&size, 1, MPI_INT, i, 1, stat);
      MPI::COMM_WORLD.Recv(&len, 1, MPI_INT, i, 1, stat);
      MPI::COMM_WORLD.Recv(name, len + 1, MPI_CHAR, i, 1, stat);
      std::cout << "Hello world: rank " << rank << " of " << size << " running on " << name << "\n";
    }

  } else {

    MPI::COMM_WORLD.Send(&rank, 1, MPI_INT, 0, 1);
    MPI::COMM_WORLD.Send(&size, 1, MPI_INT, 0, 1);
    MPI::COMM_WORLD.Send(&len, 1, MPI_INT, 0, 1);
    MPI::COMM_WORLD.Send(name, len + 1, MPI_CHAR, 0, 1);

  }*/

// Measure bcast timing for 13 GB of Friendstar data
  int e = 1806067135;
  ticks_t start = hrc_t::now();
  int *src = new int[e];
  ticks_t end = hrc_t::now();
  print_timing(start, end, "Array creation (ms):");

  start = hrc_t::now();
  if (rank == 0){
    for (i = 0; i < e; ++i){
      src[i] = rank;
    }
  }
  end = hrc_t::now();
  print_timing(start, end, "Array init (ms):");

  for (i = 0; i < 20; ++i) {
    // Just to get cleaner timings on bcast
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0){
      std::cout<<"Iteration: "<< i <<std::endl;
    }
    start = hrc_t::now();
//    MPI_Bcast(src, e, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast_chunk(src, (unsigned long) e, 0, rank);
    end = hrc_t::now();
    print_timing(start, end, "Array bcast (ms):");
  }

  delete [] src;


  MPI::Finalize();

  return (0);
}

void MPI_Bcast_chunk(int* arr, unsigned long length, int root, int rank)
{
#if DEBUG
  printf("%d MPI_B %d %lu\n", rank, root, length);
#endif

  if (length < CHUNK_SIZE)
  {
#if DEBUG
    printf("%d MPI_B no chunk %d %lu\n", rank, root, length);
#endif
    MPI_Bcast(arr, length, MPI_INT, root, MPI_COMM_WORLD);
  }
  else
  {
    unsigned long num_bcasts = length / CHUNK_SIZE;
    unsigned long cur_off = 0;
#if DEBUG
    printf("%d chunk numb %d %lu\n", rank, root, num_bcasts);
#endif
    for (int i = 0; i < num_bcasts; ++i)
    {
#if DEBUG
      printf("%d doing b %d %lu\n", rank, root, cur_off);
#endif
      MPI_Bcast(&arr[cur_off], CHUNK_SIZE, MPI_INT, root, MPI_COMM_WORLD);
      cur_off += CHUNK_SIZE;
    }
    unsigned long final_size = length - cur_off;
    assert(final_size > 0);
#if DEBUG
    printf("%d final b %d %lu %lu\n", rank, root, cur_off, final_size);
#endif
    MPI_Bcast(&arr[cur_off], final_size, MPI_INT, root, MPI_COMM_WORLD);
  }
#if DEBUG
  printf("%d done\n", rank);
#endif
}

void print_timing(
    const std::chrono::time_point<std::chrono::high_resolution_clock> &start_ms,
    const std::chrono::time_point<std::chrono::high_resolution_clock> &end_ms,
    const std::string &msg)  {
  double duration_ms, avg_duration_ms, min_duration_ms, max_duration_ms;
  duration_ms = (ms_t(end_ms - start_ms)).count();
  MPI_Reduce(&duration_ms, &min_duration_ms, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
  MPI_Reduce(&duration_ms, &max_duration_ms, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce(&duration_ms, &avg_duration_ms, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (world_proc_rank == 0){
    std::cout<<msg<<" [min max avg]ms: ["<< min_duration_ms
             << " " << max_duration_ms << " " << (avg_duration_ms / world_procs_count) << "]" <<std::endl;
  }
}
