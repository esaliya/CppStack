//
// Created by Saliya Ekanayake on 4/22/17.
//
#include "parallel_ops.hpp"
#include <mpi.h>
#include <iostream>

parallel_ops parallel_ops::initialize(int *argc, char ***argv) {
    int rank, count;
    MPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &count);
    return parallel_ops(rank, count);
}

void parallel_ops::teardown_parallelism() {
    MPI_Finalize();
}

parallel_ops::~parallel_ops() {
    // do nothing
}

parallel_ops::parallel_ops(int world_proc_rank, int world_procs_count) :
        world_proc_rank(world_proc_rank),
        world_procs_count(world_procs_count) {

}

int parallel_ops::get_world_proc_rank() const {
    return world_proc_rank;
}

int parallel_ops::get_world_procs_count() const {
    return world_procs_count;
}
