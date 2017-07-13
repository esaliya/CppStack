//
// Created by Saliya Ekanayake on 7/12/17.
//

#include <random>
#include "polynomial.hpp"
#include "polynomials.hpp"
const std::shared_ptr<polynomial> polynomial::X(create_from_long(2L));
const std::shared_ptr<polynomial> polynomial::ONE(create_from_long(1L));


/* Check with Jose, if we really need this copying(cloning) of degrees. Could have stored the shared pointer itself*/
polynomial::polynomial(std::shared_ptr<std::set<long, rev_comp_t>> degrees) : degrees((*degrees.get())){

}

polynomial::~polynomial() {

}

std::shared_ptr<polynomial> polynomial::poly_xor(std::shared_ptr<polynomial> that) {
  rev_comp_t rev_comparator = [](const long& lhs, const long& rhs) -> bool{return lhs > rhs;};
  std::set<long, rev_comp_t> dgrs0 = degrees;
  std::set<long, rev_comp_t> diff0(rev_comparator);
  std::set_difference(dgrs0.begin(), dgrs0.end(), that->degrees.begin(), that->degrees.end(),
                      std::inserter(diff0, diff0.begin()));

  std::set<long, rev_comp_t> dgrs1 = that->degrees;
  std::set<long, rev_comp_t> diff1(rev_comparator);
  std::set_difference(dgrs1.begin(), dgrs1.end(), degrees.begin(), degrees.end(),
                      std::inserter(diff1, diff1.begin()));

  dgrs1.insert(dgrs0.begin(), dgrs0.end());

  return std::shared_ptr<polynomial>(new polynomial(dgrs1));
}

std::shared_ptr<polynomial> polynomial::create_from_long(long l) {
  std::shared_ptr<std::set<long, rev_comp_t>> dgrs = create_degrees_collection();
  for (int i = 0; i < 64; i++) {
    if (((l >> i) & 1) == 1)
      dgrs->insert((long)i);
  }
  return std::shared_ptr<polynomial>(new polynomial(dgrs));
}

std::shared_ptr<polynomial> polynomial::create_from_bytes(std::vector<unsigned char> bytes, int degree) {
  std::shared_ptr<std::set<long, rev_comp_t>> dgrs = create_degrees_collection();
  for (int i = 0; i < degree; i++) {
    if (polynomials::get_bit(bytes, i))
      dgrs->insert((long)i);
  }
  dgrs->insert((long)degree);
  return std::shared_ptr<polynomial>(new polynomial(dgrs));
}

std::shared_ptr<polynomial> polynomial::create_random(int degree) {
  std::uniform_real_distribution<double> unif;
  std::default_random_engine re;
  std::vector<unsigned char> bytes((unsigned long) ((degree / 8) + 1));
  auto gen = std::bind(unif, re);
  std::generate(std::begin(bytes), std::end(bytes), gen);

  return create_from_bytes(bytes, degree);
}

std::shared_ptr<std::set<long, rev_comp_t>> polynomial::create_degrees_collection() {
  rev_comp_t rev_comparator = [](const long& lhs, const long& rhs) -> bool{return lhs > rhs;};
  return std::shared_ptr<std::set<long, rev_comp_t>>(new std::set<long, rev_comp_t>(rev_comparator));
}

int polynomial::compare(std::shared_ptr<polynomial> o) {
  return 0;
}

polynomial::polynomial() {

}









