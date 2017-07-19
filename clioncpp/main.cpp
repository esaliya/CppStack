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

int parse_args(int argc, char **argv, parallel_ops *pOps);

namespace po = boost::program_options;

int vertex_count;
int k;
int delta;
double alpha;
double epsilon;
std::string input_file;
std::string partition_file;
int node_count;
int thread_count;
int max_msg_size;

parallel_ops *p_ops;
int main(int argc, char **argv) {

  p_ops = parallel_ops::initialize(&argc, &argv);
  parse_args(argc, argv, p_ops);

  std::vector<std::shared_ptr<vertex>> *vertices = nullptr;
  p_ops->set_parallel_decomposition(input_file.c_str(), vertex_count, vertices);

  std::cout<<"---"<<(*vertices).capacity()<<std::endl;
//  std::cout << "Rank: " << p_ops->get_world_proc_rank() << " of " << p_ops->get_world_procs_count() << " Hello, World! " << std::endl;

//    Test::greet();



  for (std::vector<std::shared_ptr<vertex>>::iterator it = vertices->begin(); it != vertices->end(); ++it){
    std::cout<<(*it).get()->label<<std::endl;
  }
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

  if(vm.count(CMD_OPTION_SHORT_TC)){
    thread_count = vm[CMD_OPTION_SHORT_TC].as<int>();
  }else {
    if (is_rank0)
      std::cout<<"ERROR: Thread count not specified"<<std::endl;
    return -1;
  }

  if(vm.count(CMD_OPTION_SHORT_MMS)){
    max_msg_size = vm[CMD_OPTION_SHORT_MMS].as<int>();
  }else {
    if (is_rank0)
      std::cout<<"INFO: Max message size not specified"<<std::endl;
  }

  return 0;
}

