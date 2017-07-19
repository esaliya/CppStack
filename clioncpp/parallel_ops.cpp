//
// Created by Saliya Ekanayake on 4/22/17.
//
#include "parallel_ops.hpp"
#include "polynomial.hpp"
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <chrono>

parallel_ops * parallel_ops::initialize(int *argc, char ***argv) {
    int rank, count;
    MPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &count);
    return new parallel_ops(rank, count);
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

void parallel_ops::set_parallel_decomposition(const char *file, int global_vertx_count, std::vector<std::shared_ptr<vertex>> *&vertices) {
  // TODO - add logic to switch between different partition methods as well as txt vs binary files
  // for now let's assume simple partitioning with text files
  simple_graph_partition(file, global_vertx_count, vertices);
  decompose_among_threads(vertices);
}

void parallel_ops::simple_graph_partition(const char *file, int global_vertex_count, std::vector<std::shared_ptr<vertex>> *&vertices) {
  std::chrono::time_point<std::chrono::system_clock> start, end;

  int q = global_vertex_count/world_procs_count;
  int r = global_vertex_count % world_procs_count;
  int local_vertex_count = (world_proc_rank < r) ? q+1: q;
  int skip_vertex_count = q*world_proc_rank + (world_proc_rank < r ? world_proc_rank : r);

#ifndef NDEBUG
  std::string debug_str = (world_proc_rank==0) ? "DEBUG: simple_graph_partition: 1: q,r,localvc  [ " : " ";
  debug_str.append("[").append(std::to_string(q)).append(",").append(std::to_string(r)).append(",").append(std::to_string(local_vertex_count)).append("] ");
  debug_str = mpi_gather_string(debug_str);
  if (world_proc_rank == 0){
    std::cout<<std::endl<<std::string(debug_str).append("]")<<std::endl;
  }
//  std::cout<<"Rank: " << world_proc_rank << " q: " << q << " r: " << r << " local_vertex_count: " << local_vertex_count << std::endl;
#endif

  std::ifstream fs;
  std::string line;
  std::vector<std::string> tokens;
  vertices = new std::vector<std::shared_ptr<vertex>>((unsigned long) local_vertex_count);

  fs.open(file);

  start = std::chrono::system_clock::now();
  int local_idx;
  for (int i = 0; i < global_vertex_count; ++i) {
    getline(fs, line);
    if (i < skip_vertex_count) {
      continue;
    }
    local_idx = i-skip_vertex_count;
    boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
    (*vertices)[local_idx] = std::make_shared<vertex>(tokens);

    if (local_idx+1 == local_vertex_count){
      break;
    }
  }
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

#ifndef NDEBUG
  debug_str = (world_proc_rank==0) ? "DEBUG: simple_graph_partition: 2: graph_read elapsed  [ " : " ";
  debug_str.append(std::to_string(elapsed_seconds.count())).append(" ");
  debug_str = mpi_gather_string(debug_str);
  if (world_proc_rank == 0){
    std::cout<<std::endl<<std::string(debug_str).append("]")<<std::endl;
  }
//  std::cout<<"Rank: "<<world_proc_rank<<" graph read in "<<elapsed_seconds.count()<<" s"<<std::endl;
#endif

  fs.close();

  start = std::chrono::system_clock::now();
  find_nbrs(global_vertex_count, local_vertex_count, vertices);
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
#ifndef NDEBUG
  debug_str = (world_proc_rank==0) ? "DEBUG: simple_graph_partition: 3: find_nbrs elapsed(s)  [ " : " ";
  debug_str.append(std::to_string(elapsed_seconds.count())).append(" ");
  debug_str = mpi_gather_string(debug_str);
  if (world_proc_rank == 0){
    std::cout<<std::endl<<std::string(debug_str).append("]")<<std::endl;
  }
#endif
}

void parallel_ops::decompose_among_threads(std::vector<std::shared_ptr<vertex>> *&vertices) {

}

void parallel_ops::find_nbrs(int global_vertex_count, int local_vertex_count, std::vector<std::shared_ptr<vertex>> *&vertices) {
#ifndef NDEBUG
  int tmp_lbl = 0;
#endif

  /* Create a map to quickly lookup vertices given their label for my vertices */
  std::map<int, std::shared_ptr<vertex>> *label_to_vertex = new std::map<int, std::shared_ptr<vertex>>();
  for (std::vector<std::shared_ptr<vertex>>::iterator it = vertices->begin(); it != vertices->end(); ++it){
#ifndef NDEBUG
    tmp_lbl = (*it)->label;
#endif
    (*label_to_vertex)[(*it)->label] = *it;
  }

#ifndef NDEBUG
  /* Check vertex label to vertex map */
  std::shared_ptr<vertex> v = (*label_to_vertex)[tmp_lbl];
  std::string debug_str = (world_proc_rank==0) ? "DEBUG: find_nbrs: 1: lastvertex [ " : " ";
  debug_str.append(std::to_string(v->label)).append(" ");
  debug_str = mpi_gather_string(debug_str);
  if (world_proc_rank == 0){
    std::cout<<std::endl<<std::string(debug_str).append("]")<<std::endl;
  }
#endif

  int *local_vertex_counts = new int[world_procs_count];
  MPI_Allgather(&local_vertex_count, 1, MPI_INT, local_vertex_counts, 1, MPI_INT, MPI_COMM_WORLD);

#ifndef NDEBUG
  /* Check local vertex counts */
  debug_str = (world_proc_rank==0) ? "DEBUG: find_nbrs: 2: vcount [ " : " ";
  for (int i = 0; i < world_procs_count; ++i) {
    debug_str.append(std::to_string(local_vertex_counts[i])).append(" ");
  }
  if (world_proc_rank == 0){
    std::cout<<std::endl<<std::string(debug_str).append("]")<<std::endl;
  }
#endif

  int *local_vertex_displas = new int[world_procs_count];
  local_vertex_displas[0] = 0;
  for (int i = 1; i < world_procs_count; ++i){
    local_vertex_displas[i] = local_vertex_displas[i-1]+local_vertex_counts[i-1];
  }

#ifndef NDEBUG
  /* Check local vertex displas */
  debug_str = (world_proc_rank==0) ? "DEBUG: find_nbrs: 3: vdisplas [ " : " ";
  for (int i = 0; i < world_procs_count; ++i) {
    debug_str.append(std::to_string(local_vertex_displas[i])).append(" ");
  }
  if (world_proc_rank == 0){
    std::cout<<std::endl<<std::string(debug_str).append("]")<<std::endl;
  }
#endif



#ifdef ALL_DEBUG
  /* Allreduce string test */
  debug_str = "[hello";
  if (world_proc_rank == 0){
    debug_str = debug_str.append("wonderful");
  }
  debug_str.append(std::to_string(world_proc_rank)).append("]");
  debug_str = mpi_gather_string(debug_str);
  if (world_proc_rank == 0){
    std::cout<<std::endl<<std::string(debug_str)<<std::endl;
  }
#endif

  delete [] local_vertex_counts;
  delete label_to_vertex;
}

std::string parallel_ops::mpi_gather_string(std::string &str) {
  int len = (int) str.length();
  int *lengths = new int[world_procs_count];

  MPI_Allgather(&len, 1, MPI_INT, lengths, 1, MPI_INT, MPI_COMM_WORLD);

  int *displas = new int[world_procs_count];
  int total_length = lengths[0];
  displas[0] = 0;
  for (int i = 1; i < world_procs_count; ++i){
    displas[i] = displas[i-1]+lengths[i-1];
    total_length += lengths[i];
  }
  char *result = new char[total_length];
  MPI_Gatherv(str.c_str(), len, MPI_CHAR, result, lengths, displas, MPI_CHAR, 0, MPI_COMM_WORLD);

  std::string r = std::string(result);

  delete [] result;
  delete [] displas;
  delete [] lengths;
  return r;
}
