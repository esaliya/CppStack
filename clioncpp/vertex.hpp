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
#include <iostream>
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
    outnbr_lbl_to_world_rank = nullptr;
    delete outrank_to_send_buffer;
    outrank_to_send_buffer = nullptr;
    delete recv_buffers;
    recv_buffers = nullptr;
    delete msg;
    msg = nullptr;
    delete recvd_msgs;
    recvd_msgs = nullptr;
    delete opt_tbl;
    opt_tbl = nullptr;
  }

  // locally allocated, so no need of shared_ptr
  std::map<int,int>* outnbr_lbl_to_world_rank = nullptr;
  std::map<int,std::shared_ptr<vertex_buffer>>* outrank_to_send_buffer = nullptr;
  std::vector<std::shared_ptr<recv_vertex_buffer>>* recv_buffers = nullptr;

  message* msg = nullptr;
  std::vector<std::shared_ptr<message>>* recvd_msgs = nullptr;

  int label;
  double weight;
  unsigned int uniq_rand_seed;

  void compute(int super_step, int iter, std::shared_ptr<int> completion_vars, std::shared_ptr<std::map<int, int>> random_assignments){
    // TODO - complete compute()

    // TODO - dummy comp - list recvd messages
    if (super_step == 0){
      std::shared_ptr<short> data = std::shared_ptr<short>(new short[1](), std::default_delete<short[]>());
      data.get()[0] = (short) label;
      msg->set_data_and_msg_size(data, 1);
    } else if (super_step > 0){
      std::string str = "v";
      str.append(std::to_string(label)).append(" recvd [ ");
      for (const std::shared_ptr<message> msg : (*recvd_msgs)){
        str.append(std::to_string(msg->get(0))).append(" ");
      }
      str.append("] ss=").append(std::to_string(super_step)).append("\n");
      std::cout<<str;
    }
  }

  int prepare_send(int super_step, int shift){
    // TODO - complete prepare_send()

    // TODO - for now let's send the vertex's label
    for (const auto &kv : (*outrank_to_send_buffer)){
      std::shared_ptr<vertex_buffer> b = kv.second;
      int offset = shift + b->get_offset_factor() * msg->get_msg_size();
      msg->copy(b->get_buffer(), offset);
    }
    return msg->get_msg_size();
  }

  void process_recvd(int super_step, int shift){
    // TODO - complete process_recvd()
    for (int i = 0; i < recv_buffers->size(); ++i){
      std::shared_ptr<recv_vertex_buffer> b = (*recv_buffers)[i];
      std::shared_ptr<message> recvd_msg = (*recvd_msgs)[i];
      int recvd_msg_size = b->get_msg_size();
      recvd_msg->load(b->get_buffer(), shift+b->get_offset_factor()*recvd_msg_size, recvd_msg_size);
    }
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
  short* opt_tbl = nullptr;
  short total_sum;
  std::uniform_real_distribution<double> unif;
  std::default_random_engine re;



};


#endif //CLIONCPP_VERTEX_HPP
