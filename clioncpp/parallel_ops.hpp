//
// Created by Saliya Ekanayake on 4/23/17.
//

#ifndef CLIONCPP_PARALLEL_OPS_H
#define CLIONCPP_PARALLEL_OPS_H

//extern int get_world_proc_rank;
//extern int get_world_procs_count;
//
//void initialize(int *argc, char ***argv);
//void teardown_parallelism();

#include <boost/shared_ptr.hpp>
#include <chrono>
#include "vertex.hpp"

class parallel_ops{
public:
  int get_world_proc_rank() const;
  int get_world_procs_count() const;

  ~parallel_ops();

  void teardown_parallelism();
  void set_parallel_decomposition(const char* file, int global_vertx_count, std::vector<std::shared_ptr<vertex>> *&vertices);

  static parallel_ops * initialize(int *argc, char ***argv);

private:
  int world_proc_rank;
  int world_procs_count;

  parallel_ops(int world_proc_rank, int world_procs_count);

  void simple_graph_partition(const char* file, int global_vertex_count, std::vector<std::shared_ptr<vertex>> *&vertices);
  void decompose_among_threads(std::vector<std::shared_ptr<vertex>> *&vertices);
  void find_nbrs(int global_vertex_count, int local_vertex_count, std::vector<std::shared_ptr<vertex>> *&vertices);
  std::string mpi_gather_string(std::string &str);

  void print_timing(const std::chrono::time_point<std::chrono::high_resolution_clock> &start_ms,
                    const std::chrono::time_point<std::chrono::high_resolution_clock> &end_ms, const std::string &msg) const;
};

#endif //CLIONCPP_PARALLEL_OPS_H
