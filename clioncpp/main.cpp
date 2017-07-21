#include <iostream>
#include <vector>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <fstream>
#include "parallel_ops.hpp"
#include "constants.h"
#include "utils.hpp"

typedef std::chrono::duration<double, std::milli> ms_t;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> ticks_t;
namespace po = boost::program_options;

int parse_args(int argc, char **argv, parallel_ops *pOps);
void run_program(std::vector<std::shared_ptr<vertex>> *vertices);
void init_comp(std::vector<std::shared_ptr<vertex>> *vertices);
bool run_graph_comp(int loop_id, std::vector<std::shared_ptr<vertex>> *vertices);
void run_super_steps(std::vector<std::shared_ptr<vertex>> *vertices, int iter, int thread_id, ticks_t &start_time);
void compute(int iter, std::vector<std::shared_ptr<vertex>> *vertices, int super_step, int thread_id);
void recv_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step);
void process_recvd_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step, int thread_id);
void send_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step);
void finalize_iteration(std::vector<std::shared_ptr<vertex>> *vertices, int thread_id);
void finalize_iterations(std::vector<std::shared_ptr<vertex>> *vertices);

int vertex_count;
int k;
int delta;
double alpha;
double epsilon;
std::string input_file;
std::string partition_file;

// default values;
int node_count = 1;
int thread_count = 1;
int max_msg_size = 500;
int parallel_instance_id = 0;
int parallel_instance_count = 1;

parallel_ops *p_ops;

int main(int argc, char **argv) {

  p_ops = parallel_ops::initialize(&argc, &argv);
  int ret = parse_args(argc, argv, p_ops);
  if (ret < 0){
    p_ops->teardown_parallelism();
    return ret;
  }

  std::vector<std::shared_ptr<vertex>> *vertices = nullptr;
  p_ops->set_parallel_decomposition(input_file.c_str(), vertex_count, vertices);

//  std::cout<<"---"<<(*vertices).capacity()<<std::endl;
//  std::cout << "Rank: " << p_ops->get_world_proc_rank() << " of " << p_ops->get_world_procs_count() << " Hello, World! " << std::endl;

//    Test::greet();

  delete vertices;
  p_ops->teardown_parallelism();
  return 0;
}

int parse_args(int argc, char **argv, parallel_ops *p_ops) {
  // Declare the supported options.
  po::options_description desc(PROGRAM_NAME);
  desc.add_options()
      ("help", "produce help message")
      (CMD_OPTION_SHORT_VC, po::value<int>(), CMD_OPTION_DESCRIPTION_VC)
      (CMD_OPTION_SHORT_K, po::value<int>(), CMD_OPTION_DESCRIPTION_K)
      (CMD_OPTION_SHORT_DELTA, po::value<int>(), CMD_OPTION_DESCRIPTION_DELTA)
      (CMD_OPTION_SHORT_ALPHA, po::value<double>(), CMD_OPTION_DESCRIPTION_ALPHA)
      (CMD_OPTION_SHORT_EPSILON, po::value<double>(), CMD_OPTION_DESCRIPTION_EPSILON)
      (CMD_OPTION_SHORT_INPUT, po::value<std::string>(), CMD_OPTION_DESCRIPTION_INPUT)
      (CMD_OPTION_SHORT_PARTS, po::value<std::string>(), CMD_OPTION_DESCRIPTION_PARTS)
      (CMD_OPTION_SHORT_NC, po::value<int>(), CMD_OPTION_DESCRIPTION_NC)
      (CMD_OPTION_SHORT_TC, po::value<int>(), CMD_OPTION_DESCRIPTION_TC)
      (CMD_OPTION_SHORT_MMS, po::value<int>(), CMD_OPTION_DESCRIPTION_MMS)
      (CMD_OPTION_SHORT_PI, po::value<int>(), CMD_OPTION_DESCRIPTION_PI)
      (CMD_OPTION_SHORT_PIC, po::value<int>(), CMD_OPTION_DESCRIPTION_PIC)
      ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, (const char *const *) argv, desc), vm);
  po::notify(vm);

  bool is_rank0 = p_ops->get_world_proc_rank() == 0;
  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  if (vm.count(CMD_OPTION_SHORT_VC)){
    vertex_count = vm[CMD_OPTION_SHORT_VC].as<int>();
  } else {
    if (is_rank0)
      std::cout<<"ERROR: Vertex count not specified"<<std::endl;
    return -1;
  }

  if(vm.count(CMD_OPTION_SHORT_K)){
    k = vm[CMD_OPTION_SHORT_K].as<int>();
  } else {
    if (is_rank0)
      std::cout<<"ERROR: K is not specified"<<std::endl;
    return -1;
  }

  if(vm.count(CMD_OPTION_SHORT_DELTA)){
    delta = vm[CMD_OPTION_SHORT_DELTA].as<int>();
  } else {
    if (is_rank0)
      std::cout<<"ERROR: Delta not specified"<<std::endl;
    return -1;
  }

  if(vm.count(CMD_OPTION_SHORT_ALPHA)){
    alpha = vm[CMD_OPTION_SHORT_ALPHA].as<double>();
  } else {
    if (is_rank0)
      std::cout<<"ERROR: Alpha not specified"<<std::endl;
    return -1;
  }

  if(vm.count(CMD_OPTION_SHORT_EPSILON)){
    epsilon = vm[CMD_OPTION_SHORT_EPSILON].as<double>();
  } else {
    if (is_rank0)
      std::cout<<"ERROR: Epsilon not specified"<<std::endl;
    return -1;
  }

  if (vm.count(CMD_OPTION_SHORT_INPUT)){
    input_file = vm[CMD_OPTION_SHORT_INPUT].as<std::string>();
  }else {
    if (is_rank0)
      std::cout<<"ERROR: Input file not specified"<<std::endl;
    return -1;
  }

  if(vm.count(CMD_OPTION_SHORT_PARTS)){
    partition_file = vm[CMD_OPTION_SHORT_PARTS].as<std::string>().c_str();
  }else {
    if (is_rank0)
      std::cout<<"INFO: Partition file not specified, assuming uniform rank partitioning"<<std::endl;
  }

  if(vm.count(CMD_OPTION_SHORT_NC)){
    node_count = vm[CMD_OPTION_SHORT_NC].as<int>();
  }else {
    if (is_rank0)
      std::cout<<"ERROR: Node count not specified"<<std::endl;
    return -1;
  }
  p_ops->node_count = node_count;

  if(vm.count(CMD_OPTION_SHORT_TC)){
    thread_count = vm[CMD_OPTION_SHORT_TC].as<int>();
  }else {
    if (is_rank0)
      std::cout<<"INFO: Thread count not specified, assuming "<<thread_count<<std::endl;
  }
  p_ops->thread_count = thread_count;

  if(vm.count(CMD_OPTION_SHORT_MMS)){
    max_msg_size = vm[CMD_OPTION_SHORT_MMS].as<int>();
  }else {
    if (is_rank0)
      std::cout<<"INFO: Max message size not specified, assuming "<<max_msg_size<<std::endl;
  }
  p_ops->max_msg_size = max_msg_size;

  if(vm.count(CMD_OPTION_SHORT_PI)){
    parallel_instance_id = vm[CMD_OPTION_SHORT_PI].as<int>();
  }else {
    if (is_rank0)
      std::cout<<"INFO: Parallel instance id not specified, assuming "<<parallel_instance_id<<std::endl;
  }
  p_ops->parallel_instance_id = parallel_instance_id;

  if(vm.count(CMD_OPTION_SHORT_PIC)){
    parallel_instance_count = vm[CMD_OPTION_SHORT_PIC].as<int>();
  }else {
    if (is_rank0)
      std::cout<<"INFO: Parallel instance count not specified, assuming "<<parallel_instance_count<<std::endl;
  }
  p_ops->parallel_instance_count = parallel_instance_count;

  return 0;
}

