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
  std::cout<<"Rank: " << world_proc_rank << " q: " << q << " r: " << r << " local_vertex_count: " << local_vertex_count << std::endl;
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
  std::cout<<"Rank: "<<world_proc_rank<<" graph read in "<<elapsed_seconds.count()<<" s"<<std::endl;
#endif

  fs.close();

  start = std::chrono::system_clock::now();
  find_nbrs(global_vertex_count, local_vertex_count, vertices);
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
#ifndef NDEBUG
  std::cout<<"Rank: "<<world_proc_rank<<" find neighbors "<<elapsed_seconds.count()<<" s"<<std::endl;
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
  std::shared_ptr<vertex> v = (*label_to_vertex)[tmp_lbl];
  std::cout<<"find_nbrs: debug1: Rank: "<<world_proc_rank<<" "<<v->label<<std::endl;
#endif



  delete label_to_vertex;
}
