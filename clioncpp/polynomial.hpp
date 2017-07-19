//
// Created by Saliya Ekanayake on 7/12/17.
//

#ifndef CLIONCPP_POLYNOMIAL_HPP
#define CLIONCPP_POLYNOMIAL_HPP


#include <set>
#include <boost/shared_ptr.hpp>
#include <vector>

typedef bool (*rev_comp_t)(const long&, const long&);

class polynomial {
public:
  static const long Q = 2L;
  static const std::shared_ptr<polynomial> X;
  static const std::shared_ptr<polynomial> ONE;

  ~polynomial();

private:
  static std::shared_ptr<polynomial> create_from_long(long l);
  static std::shared_ptr<polynomial> create_from_bytes(std::vector<unsigned char> bytes, int degree);
  static std::shared_ptr<polynomial> create_random(int degree);
  static std::set<long, rev_comp_t> *create_degrees_collection();

  std::set<long, rev_comp_t> *degrees = nullptr;

  polynomial();
  polynomial(std::set<long, rev_comp_t> *degrees);
  int compare(std::shared_ptr<polynomial> o);
  std::shared_ptr<polynomial> poly_xor(std::shared_ptr<polynomial> that);
  long get_degree();
  bool is_empty();
  bool has_degree(long k);
};


#endif //CLIONCPP_POLYNOMIAL_HPP
