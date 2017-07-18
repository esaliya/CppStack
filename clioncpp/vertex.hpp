//
// Created by Saliya Ekanayake on 6/24/17.
//

#ifndef CLIONCPP_VERTEX_HPP
#define CLIONCPP_VERTEX_HPP


#include <map>
#include <vector>
#include <random>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "recv_vertex_buffer.hpp"
#include "message.hpp"
#include "galois_field.hpp"

class vertex {
public:
  vertex(int label, double weight, int* outnbrs, int outnbrs_length){
    this->label = label;
    this->weight = weight;
    outnbr_lbl_to_world_rank = new std::map<int,int>();
    outrank_to_send_buffer = new std::map<int,std::shared_ptr<vertex_buffer>>();
    recv_buffers = new std::vector<std::shared_ptr<recv_vertex_buffer>>();
    msg = new message();
    recvd_msgs = new std::vector<std::shared_ptr<message>>();

    for (int i = 0; i < outnbrs_length; ++i) {
      (*outnbr_lbl_to_world_rank)[outnbrs[i]] = -1;
    }
  }

  vertex(std::vector<std::string> &tokens){
    this->label = std::stoi(tokens[0]);
    this->weight = std::stof(tokens[1]);
    outnbr_lbl_to_world_rank = new std::map<int,int>();
    outrank_to_send_buffer = new std::map<int,std::shared_ptr<vertex_buffer>>();
    recv_buffers = new std::vector<std::shared_ptr<recv_vertex_buffer>>();
    msg = new message();
    recvd_msgs = new std::vector<std::shared_ptr<message>>();

    for (int i = 2; i < tokens.size(); ++i){
      (*outnbr_lbl_to_world_rank)[std::stoi(tokens[i])] = -1;
    }
  }

  ~vertex(){
    delete outnbr_lbl_to_world_rank;
    delete outrank_to_send_buffer;
    delete recv_buffers;
    delete msg;
    delete recvd_msgs;
    delete opt_tbl;
  }

  // locally allocated, so no need of shared_ptr
  std::map<int,int>* outnbr_lbl_to_world_rank;
  std::map<int,std::shared_ptr<vertex_buffer>>* outrank_to_send_buffer;
  std::vector<std::shared_ptr<recv_vertex_buffer>>* recv_buffers;

  message* msg;
  std::vector<std::shared_ptr<message>>* recvd_msgs;

  int label;
  double weight;
  unsigned int uniq_rand_seed;

  void compute(int super_step, int iter, int* completion_vars, std::map<int, int> random_assignments){
    // TODO - complete compute()
  }

  int prepare_send(int super_step, int shift){
    // TODO - complete prepare_send()
    return -27;
  }

  void process_recvd(int super_step, int shift){
    // TODO - complete process_recvd()
  }

  void init(int k , int r, std::shared_ptr<galois_field> gf){
    this->k = k;
    this->gf = gf;
    opt_tbl = new short[k+1];
  }

  void reset(){
    // TODO - complete reset()
  }

  void finalize_iteration(){
    total_sum = (short) (*gf).add(total_sum, opt_tbl[k]);
  }

  bool finalize_iterations(){
    return total_sum > 0;
  }

private:
  int k;
  std::shared_ptr<galois_field> gf;
  short* opt_tbl;
  short total_sum;
  std::uniform_real_distribution<double> unif;
  std::default_random_engine re;



};


#endif //CLIONCPP_VERTEX_HPP
