//
// Created by Saliya Ekanayake on 7/8/17.
//

#include <algorithm>

#ifndef CLIONCPP_MESSAGE_HPP
#define CLIONCPP_MESSAGE_HPP

#endif //CLIONCPP_MESSAGE_HPP

class message{
public:
  message(){}
  ~message(){}

  void copy(short* buffer, int offset){
    buffer[offset] = dim_a;
    std::copy(data, data+dim_a, &buffer[offset+1]);
  }

  void load(short* buffer, int offset, int recvd_msg_size){
    msg_size = recvd_msg_size;
    dim_a = buffer[offset];
    read_offset = offset+1;
    this->buffer = buffer;
  }

  short get(int i){
    return buffer[read_offset+i];
  }

  int get_msg_size(){
    return msg_size;
  }

  void set_data_and_msg_size(short* data, int msgSize) {
    this->data = data;
    // +1 to store dimension
    this->msg_size = msgSize+1;
  }


private:
  int msg_size;
  short* data;
  short dim_a;
  int read_offset;
  short* buffer;

};