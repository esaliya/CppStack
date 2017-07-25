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

int parse_args(int argc, char **argv);
void run_program(std::vector<std::shared_ptr<vertex>> *vertices);
void init_comp(std::vector<std::shared_ptr<vertex>> *vertices);
bool run_graph_comp(int loop_id, std::vector<std::shared_ptr<vertex>> *vertices);
void init_loop(std::vector<std::shared_ptr<vertex>> *vertices);
void run_super_steps(std::vector<std::shared_ptr<vertex>> *vertices, int iter, int thread_id, ticks_t &start_time);
void compute(int iter, std::vector<std::shared_ptr<vertex>> *vertices, int super_step, int thread_id);
void recv_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step);
void process_recvd_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step, int thread_id);
void send_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step);
void finalize_iteration(std::vector<std::shared_ptr<vertex>> *vertices, int thread_id);
void finalize_iterations(std::vector<std::shared_ptr<vertex>> *vertices);

void pretty_print_config(std::string &str);

int vertex_count;
int k;
int delta;
double alpha;
double epsilon;
std::string input_file;
std::string partition_file;

int two_raised_to_k;
std::shared_ptr<galois_field> gf = nullptr;
int max_iterations;
std::shared_ptr<std::map<int,int>> random_assignments = nullptr;
std::shared_ptr<int> completion_vars = nullptr;

// default values;
int node_count = 1;
int thread_count = 1;
int max_msg_size = 500;
int parallel_instance_id = 0;
int parallel_instance_count = 1;

bool is_rank0 = false;

parallel_ops *p_ops;

int main(int argc, char **argv) {

  p_ops = parallel_ops::initialize(&argc, &argv);
  int ret = parse_args(argc, argv);
  if (ret < 0){
    p_ops->teardown_parallelism();
    return ret;
  }

  is_rank0 = (p_ops->get_world_proc_rank() == 0);

  std::vector<std::shared_ptr<vertex>> *vertices = nullptr;
  p_ops->set_parallel_decomposition(input_file.c_str(), vertex_count, vertices);
  run_program(vertices);
  delete vertices;
  p_ops->teardown_parallelism();
  return 0;
}

int parse_args(int argc, char **argv) {
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

  if(vm.count(CMD_OPTION_SHORT_PIC)){
    parallel_instance_count = vm[CMD_OPTION_SHORT_PIC].as<int>();
  }else {
    if (is_rank0)
      std::cout<<"INFO: Parallel instance count not specified, assuming "<<parallel_instance_count<<std::endl;
  }

  return 0;
}

void run_program(std::vector<std::shared_ptr<vertex>> *vertices) {
  ticks_t start_prog = std::chrono::system_clock::now();
  std::time_t start_prog_time = std::chrono::system_clock::to_time_t(start_prog);
  std::string print_str = "\nINFO: Graph computation started on ";
  print_str.append(std::ctime(&start_prog_time));
  pretty_print_config(print_str);
  if (is_rank0){
    std::cout<<print_str;
  }

  // get number of iterations for a target error bound (epsilon)
  double prob_success = 0.2;
  int iter = (int) round(log(epsilon) / log(1 - prob_success));

  print_str = "INFO: ";
  print_str.append(std::to_string(iter)).append(" assignments will be evaluated for epsilon ")
      .append(std::to_string(epsilon)).append("\n");
  if (is_rank0){
    std::cout<<print_str;
  }

  ticks_t start_loops = std::chrono::high_resolution_clock::now();
  bool found_path = false;
  init_comp(vertices);

  for(int i = 0; i < iter; ++i){
    print_str = "  INFO: Start of loop ";
    print_str.append(std::to_string(i)).append("\n");
    if (is_rank0) std::cout<<print_str;

    ticks_t start_loop = std::chrono::high_resolution_clock::now();
    found_path = run_graph_comp(i, vertices);
    if (found_path){
      break;
    }
    ticks_t end_loop = std::chrono::high_resolution_clock::now();
    print_str = "  INFO: End of loop ";
    print_str.append(std::to_string(i)).append(" duration (ms) ").
        append(std::to_string((ms_t(end_loop - start_loop)).count())).append("\n");
    if(is_rank0) std::cout<<print_str;
  }

  ticks_t end_loops = std::chrono::high_resolution_clock::now();
  print_str = "INFO: Graph ";
  print_str.append(found_path ? "contains " : "does not contain ").append("a ");
  print_str.append(std::to_string(k)).append("-path");
  if (is_rank0) std::cout<<print_str<<std::endl;

  print_str = "INFO: Loops total time (ms)";
  print_str.append(std::to_string((ms_t(end_loops - start_loops)).count())).append("\n");

  ticks_t end_prog = std::chrono::high_resolution_clock::now();
  std::time_t end_prog_time = std::chrono::system_clock::to_time_t(end_prog);

  print_str.append("INFO: Graph computation ended on ");
  print_str.append(std::ctime(&start_prog_time));

  if(is_rank0){
    std::cout<<print_str;
  }
}

void init_comp(std::vector<std::shared_ptr<vertex>> *vertices) {
  two_raised_to_k = 1 << k;
  max_iterations = k - 1;
  random_assignments = std::make_shared<std::map<int,int>>();
  completion_vars = std::shared_ptr<int>(new int[k-1](), std::default_delete<int[]>());
}

bool run_graph_comp(int loop_id, std::vector<std::shared_ptr<vertex>> *vertices) {
  // TODO - going with the skeleton code
  init_loop(vertices);
  ticks_t start_ticks = std::chrono::high_resolution_clock::now();

  int iterations_per_parallel_instance = 1;
  for (int iter = 0; iter < iterations_per_parallel_instance; ++iter){
    int final_iter = iter+(parallel_instance_id*iterations_per_parallel_instance);
    int thread_id = 0;
    run_super_steps(vertices, thread_id, final_iter, start_ticks);
  }
  return false;
}

void init_loop(std::vector<std::shared_ptr<vertex>> *vertices) {
  // TODO - initi_loop
  long long per_loop_random_seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  MPI_Bcast(&per_loop_random_seed, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

}

void run_super_steps(std::vector<std::shared_ptr<vertex>> *vertices, int iter, int thread_id, ticks_t &start_time) {
  int worker_steps = 2;// TODO - correct worker steps
  for (int ss = 0; ss < worker_steps; ++ss){
    if (ss > 0){
      recv_msgs(vertices, ss);
      process_recvd_msgs(vertices, ss, thread_id);
    }

    compute(iter, vertices, ss, thread_id);

    if (ss< worker_steps - 1){
      send_msgs(vertices, ss);
    }
  }

  finalize_iteration(vertices, thread_id);
}

void compute(int iter, std::vector<std::shared_ptr<vertex>> *vertices, int super_step, int thread_id) {
  for (const auto &vertex : (*vertices)){
    vertex->compute(super_step, iter, nullptr, nullptr);
  }
}

void recv_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step) {
  p_ops->recv_msgs();
}

void process_recvd_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step, int thread_id) {
  for (const auto &vertex : (*vertices)){
    vertex->process_recvd(super_step, p_ops->BUFFER_OFFSET);
  }
}

void send_msgs(std::vector<std::shared_ptr<vertex>> *vertices, int super_step) {
  int msg_size = -1;
  for (const auto &vertex : (*vertices)){
    msg_size = vertex->prepare_send(super_step, p_ops->BUFFER_OFFSET);
  }
  p_ops->send_msgs(msg_size);
}

void finalize_iteration(std::vector<std::shared_ptr<vertex>> *vertices, int thread_id) {

}

void finalize_iterations(std::vector<std::shared_ptr<vertex>> *vertices) {

}

void pretty_print_config(std::string &str){
  std::string params [] = {"Input File",
                           "Global Vertex Count",
                           "K",
                           "Epsilon",
                           "Delta",
                           "Alpha Max",
                           "Parallel Pattern"};
}


