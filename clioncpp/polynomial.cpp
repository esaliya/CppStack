//
// Created by Saliya Ekanayake on 7/12/17.
//

#include <random>
#include "polynomial.hpp"
#include "polynomials.hpp"
const std::shared_ptr<polynomial> polynomial::X(create_from_long(2L));
const std::shared_ptr<polynomial> polynomial::ONE(create_from_long(1L));

polynomial::polynomial() {

}

/* Check with Jose, if we really need this copying(cloning) of degrees.
 * Could have stored the shared pointer itself. I am doing it as pointers*/
polynomial::polynomial(std::set<long, rev_comp_t> *degrees) : degrees(degrees){

}

polynomial::~polynomial() {
  delete degrees;
}

std::shared_ptr<polynomial> polynomial::poly_xor(std::shared_ptr<polynomial> that) {
  rev_comp_t rev_comparator = [](const long& lhs, const long& rhs) -> bool{return lhs > rhs;};
  std::set<long, rev_comp_t> dgrs0 = *degrees;
  std::set<long, rev_comp_t> diff0(rev_comparator);
  std::set_difference(dgrs0.begin(), dgrs0.end(), (*that->degrees).begin(), (*that->degrees).end(),
                      std::inserter(diff0, diff0.begin()));

  std::set<long, rev_comp_t> *dgrs1 = new std::set<long, rev_comp_t>(*(that->degrees));
  std::set<long, rev_comp_t> diff1(rev_comparator);
  std::set_difference((*dgrs1).begin(), (*dgrs1).end(), (*degrees).begin(), (*degrees).end(),
                      std::inserter(diff1, diff1.begin()));

  (*dgrs1).insert(dgrs0.begin(), dgrs0.end());

  return std::shared_ptr<polynomial>(new polynomial(dgrs1));
}

std::shared_ptr<polynomial> polynomial::create_from_long(long l) {
  std::set<long, rev_comp_t> *dgrs = create_degrees_collection();
  for (int i = 0; i < 64; i++) {
    if (((l >> i) & 1) == 1)
      dgrs->insert((long)i);
  }
  return std::shared_ptr<polynomial>(new polynomial(dgrs));
}

std::shared_ptr<polynomial> polynomial::create_from_bytes(std::vector<unsigned char> bytes, int degree) {
  std::set<long, rev_comp_t> *dgrs = create_degrees_collection();
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

std::set<long, rev_comp_t> *polynomial::create_degrees_collection() {
  rev_comp_t rev_comparator = [](const long& lhs, const long& rhs) -> bool{return lhs > rhs;};
  return new std::set<long, rev_comp_t>(rev_comparator);
}

int polynomial::compare(std::shared_ptr<polynomial> o) {
  long deg = get_degree();
  long odeg = o->get_degree();
  int cmp = (deg < odeg ? -1 : (deg == odeg ? 0 : 1));
  std::shared_ptr<polynomial> x = poly_xor(o);
  if (x->is_empty()) return 0;
  return has_degree(x->get_degree()) ? 1 : -1;
}

long polynomial::get_degree() {
  if (degrees->empty()){
    return -1L;
  }
  return *(degrees->begin());
}

bool polynomial::is_empty() {
  return degrees->empty();
}

bool polynomial::has_degree(long k) {
  return (degrees->find(k) != degrees->end());
}











