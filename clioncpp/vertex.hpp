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
    outrank_to_send_buffer = new std::map<int,short*>();
    recv_buffers = new std::vector<recv_vertex_buffer>();
    msg = new message();
    recvd_msgs = new std::vector<message>();

    for (int i = 0; i < outnbrs_length; ++i) {
      (*outnbr_lbl_to_world_rank)[outnbrs[i]] = -1;
    }
  }

  vertex(std::string vertex_line){
    std::vector<std::string> tokens;
    boost::split(tokens, vertex_line, boost::is_any_of(" "), boost::token_compress_on);
    this->label = std::stoi(tokens[0]);
    this->weight = std::stod(tokens[1]);
    outnbr_lbl_to_world_rank = new std::map<int,int>();
    outrank_to_send_buffer = new std::map<int,short*>();
    recv_buffers = new std::vector<recv_vertex_buffer>();
    msg = new message();
    recvd_msgs = new std::vector<message>();

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
    delete gf;
    delete opt_tbl;
  }

  std::map<int,int>* outnbr_lbl_to_world_rank;
  std::map<int,short*>* outrank_to_send_buffer;
  std::vector<recv_vertex_buffer>* recv_buffers;

  message* msg;
  std::vector<message>* recvd_msgs;

  int label;
  double weight;
  unsigned int uniq_rand_seed;

  void finalize_iteration(){
    total_sum = (short) (*gf).add(total_sum, opt_tbl[k]);
  }

  bool finalize_iterations(){
    return total_sum > 0;
  }

private:
  int k;
  galois_field* gf;
  short* opt_tbl;
  short total_sum;
  std::uniform_real_distribution<double> unif;
  std::default_random_engine re;



};


#endif //CLIONCPP_VERTEX_HPP
