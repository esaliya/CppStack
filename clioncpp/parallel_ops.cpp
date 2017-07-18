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

void parallel_ops::set_parallel_decomposition(const char *file, int global_vertx_count, std::vector<std::shared_ptr<vertex>> *vertices) {
  // TODO - add logic to switch between different partition methods as well as txt vs binary files
  // for now let's assume simple partitioning with text files
  simple_graph_partition(file, global_vertx_count, vertices);
//  decompose_among_threads(vertices);
}

void parallel_ops::simple_graph_partition(const char *file, int global_vertex_count, std::vector<std::shared_ptr<vertex>> *vertices) {
  int q = global_vertex_count/world_procs_count;
  int r = global_vertex_count % world_procs_count;
  int my_vertex_count = (world_proc_rank < r) ? q+1: q;
  int skip_vertex_count = q*world_proc_rank + (world_proc_rank < r ? world_proc_rank : r);

  vertices = new std::vector<std::shared_ptr<vertex>>((unsigned long) my_vertex_count);

#ifndef NDEBUG
  std::cout<<"Rank: " << world_proc_rank << " q: " << q << " r: " << r << " my_vertex_count: " << my_vertex_count << std::endl;
#endif

  std::ifstream fs;
  std::string line;
  fs.open(file);

  std::vector<std::string> tokens;
  int local_idx;

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  for (int i = 0; i < global_vertex_count; ++i) {
    getline(fs, line);
    if (i < skip_vertex_count) {
      continue;
    }
    local_idx = i-skip_vertex_count;
    boost::split(tokens, line, boost::is_any_of(" "), boost::token_compress_on);
    (*vertices)[local_idx] = std::make_shared<vertex>(tokens);

    if (local_idx+1 == my_vertex_count){
      break;
    }
  }
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

#ifndef NDEBUG
  std::cout<<"Rank: "<<world_proc_rank<<" graph read in "<<elapsed_seconds.count()<<" s"<<std::endl;
#endif

  fs.close();

  /*long t = System.currentTimeMillis();
  try (BufferedReader reader = Files.newBufferedReader(Paths.get(file))){
    String line;
    while ((line = reader.readLine()) != null){
      if (Strings.isNullOrEmpty(line)) continue;
      if (readCount < skip_vertex_count){
        ++readCount;
        continue;
      }
      vertices[i] = new Vertex(readCount, line);
      ++i;
      ++readCount;
      if (i == my_vertex_count) break;
    }

  } catch (IOException e) {
    e.printStackTrace();
  }
  if (debug3){
    System.out.println("Rank: " + world_proc_rank + " readgraph: "+ (System.currentTimeMillis() - t) + " ms");
  }

  t = System.currentTimeMillis();
  findNeighbors(global_vertex_count, vertices);
  if (debug3 && world_proc_rank == 0){
    System.out.println("Rank: 0 findNbrs: "+ (System.currentTimeMillis() - t) + " ms");
  }
  return vertices;*/
}

void parallel_ops::decompose_among_threads(std::vector<std::shared_ptr<vertex>> &vertices) {

}
