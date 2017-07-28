#include <iostream>
#include "mpi.h"

int main(int argc, char *argv[]) {
  int i, rank, size, len;
  char name[MPI_MAX_PROCESSOR_NAME];
  MPI::Status stat;

  MPI::Init(argc, argv);

  size = MPI::COMM_WORLD.Get_size();
  rank = MPI::COMM_WORLD.Get_rank();
  MPI::Get_processor_name(name, len);

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

  }

  MPI::Finalize();

  return (0);
}
