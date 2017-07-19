#include <omp.h>
#include <stdio.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <random>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/foreach.hpp>
#include <set>
#include "test_map_class.hpp"

void vector_test(){
  std::shared_ptr<std::vector<int>> v = std::make_shared<std::vector<int>>();
  v->push_back(10);
  if (v->size() > 0){
    ++(*v)[0];
  }
  std::cout<<(*v)[0]<<std::endl;

  std::shared_ptr<std::vector<int>> w = std::make_shared<std::vector<int>>();
  w->push_back(23);
  w->push_back(45);

  std::copy(w->begin(), w->end(), std::back_inserter(*v));

  for (const auto &val : (*v)){
    std::cout<<val<<std::endl;
  }

}

void sizeof_test(){
  int x = 1000000000;
  std::cout<< sizeof(x)<<" "<<x<<std::endl;
}


void assign_to_point_test2_internal_internal(std::vector<std::shared_ptr<test_map_class>> *&vec){
  vec = new std::vector<std::shared_ptr<test_map_class>>(5);
  (*vec)[0] = std::make_shared<test_map_class>(999);
}

void assign_to_pointer_test2_internal(std::vector<std::shared_ptr<test_map_class>> *&vec){
  assign_to_point_test2_internal_internal(vec);
  (*vec)[1] = std::make_shared<test_map_class>(888);
}

void assign_to_pointer_test2(){
  std::vector<std::shared_ptr<test_map_class>> *vec = nullptr;
  assign_to_pointer_test2_internal(vec);
  std::cout<<((*vec)[0].get())->get_yy()<<std::endl;
  std::cout<<((*vec)[1].get())->get_yy()<<std::endl;
  delete vec;
}

void assign_to_pointer_test_internal(std::vector<int> **v){
  (*v) = new std::vector<int>();
  (*v)->push_back(10);
}

void assign_to_pointer_test(){
  std::vector<int> *WECK = nullptr;
  std::vector<int> **vec = &WECK;
  assign_to_pointer_test_internal(vec);
  std::cout<<(**vec)[0]<<std::endl;
  delete *vec;
}


void pass_by_ref_helper(std::vector<int> &vec){
  vec[0] = 23;
  vec[1] = 43;
}

void pass_by_ref_test(){
  std::vector<int> *vec = new std::vector<int>(4);
  pass_by_ref_helper((*vec));
  for (std::vector<int>::iterator it = vec->begin(); it != vec->end(); ++it){
    std::cout<<(*it)<<std::endl;
  }
}

void test_set_clear(){
  std::set<int> myset;
  std::set<int> myset2;
  std::set<int> diff;
  for (int i=1; i<=10; i++) {
    myset.insert(i*10);
    if (i < 4) {
      myset2.insert((i + 3) * 10);
    }
  }
  myset2.insert(90);

//  myset.erase(myset2.begin(), myset2.end());

  std::set_difference(myset.begin(), myset.end(), myset2.begin(), myset2.end(), std::inserter(diff, diff.begin()));
  for (std::set<int>::iterator it=diff.begin(); it!=diff.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

}

void test_object_creation_and_shared_ptr(){
  std::shared_ptr<test_map_class> shared_ptr_to_obj = test_map_class::create_object_and_return_shared_ptr();
}

void test_shared_ptr_for_set(){
  std::shared_ptr<std::set<long, rev_comp_t>> sp_set = test_map_class::create_degrees_collection();
  sp_set->insert(37);
  sp_set->insert(68);
  sp_set->insert(22);
  for (std::set<long, rev_comp_t>::iterator it = sp_set->begin(); it != sp_set->end(); ++it){
    std::cout<<(*it)<<std::endl;
  }
}

void test_comparator(){
  auto reverse_comparator = [](const int& lhs, const int& rhs) -> bool{return lhs > rhs;};
  std::set<int, decltype(reverse_comparator)> myset(reverse_comparator);
  myset.insert(10);
  myset.insert(20);
  for (std::set<int>::iterator it = myset.begin(); it != myset.end(); ++it){
    std::cout<<(*it);
  }

  for (std::set<int>::iterator it = myset.begin(); it != myset.end(); ++it){
    std::cout<<(*it);
  }
}

void test_loop(){
  int field_size = 10;
  for (int i = 0; i < field_size; ++i) {
    for (int j = 0; j < field_size; ++j) {
      if (i == 0 || j == 0) {
        std::cout<<i << " " << j << std::endl;
        continue;
      }
      int z = 100;
      std::cout<<z<<std::endl;
    }
  }
}

void test_method_for_map(){
  test_map_class tmc(43);
  /* Should print 22 and 57 (not 43 or any other number) */
  tmc.test_object_creation_and_shared_ptr();
//  test_map_class::add_and_print_element(10,20);
//  test_map_class tmc;
//  tmc.add_element_to_vector_and_print(100);
}

void test_string(){
  std::string split_me( "hello world  how are   you" );

  std::vector<std::string> tokens;
  boost::split( tokens, split_me, boost::is_any_of(" "), boost::token_compress_on);

  std::string split_me_too( "yes yes i'm ready" );
  boost::split( tokens, split_me_too, boost::is_any_of(" "), boost::token_compress_on);

  std::cout << tokens.size() << " tokens" << std::endl;
//  BOOST_FOREACH( const std::string& i, tokens ) {
//    std::cout << "'" << i << "'" << std::endl;
//  }

  for (std::string v : tokens){
    std::cout << "'" << v << "'" << std::endl;
  }

  std::string number = "124";
  std::cout<<std::stoi(number);

}


void test_rand(){
  std::uniform_real_distribution<double> unif;
  std::default_random_engine re;
  std::default_random_engine re2;
  re.seed(10088388);
  re2.seed(100883883);
  for (int i = 0; i < 10; ++i) {
    std::cout << unif(re) << " " << unif(re2) << std::endl;
  }
}

void test_array(){
  int* arr = new int[10];
  arr[1] = 23;
  std::cout<<arr[1]<<std::endl;
  delete[] arr;

  std::vector<int> vint(2);
  vint[1] =13;
  std::cout<<vint[1]<<std::endl;

  std::vector<int>* vint2 = new std::vector<int>(2);
  (*vint2)[1] =33;
  std::cout<<(*vint2)[1]<<std::endl;

  // Seems it's not necessary to do this shared pointers for vectors
  std::shared_ptr<std::vector<int>> shared_vector = std::make_shared<std::vector<int>>(2);


}


void test_shard_ptr(std::shared_ptr<std::map<int,int>> x){
  (*x)[2] = 40;
}

void test_map(){
  std::map<int, int> *number_map = new std::map<int,int>();
  std::shared_ptr<std::map<int,int>> smart_map(new std::map<int,int>());
  std::shared_ptr<std::map<int,int>> smart_map2 = std::make_shared<std::map<int,int>>();

  (*smart_map)[2] = 30;
  test_shard_ptr(smart_map);
  (*number_map)[2] = 20;
  int* arr = new int[4];
  arr[1] = 10;
//  number_map[2] = 20;
//  number_map[4] = 40;
  std::cout<<(*number_map)[2]<<" "<<arr[1]<<" "<<(*smart_map)[1]<<std::endl;
  delete[] arr;
}

void test(){
  short* a = (short*) malloc(sizeof(short)*3);
  short* b = new short[5];

  for (int i = 0; i < 3; ++i){
    a[i] = (short) i;
  }

  for (int i = 0; i < 5; ++i){
    b[i] = (short) (i+5);
  }


  std::copy(a, a+3, &b[2]);

  for (int i = 0; i < 5; ++i){
    std::cout<<b[i]<<" ";
  }
}

int main() {
  vector_test();
//  sizeof_test();
//  assign_to_pointer_test2();
//  assign_to_pointer_test();
//  pass_by_ref_test();
//  test_set_clear();
//  test_object_creation_and_shared_ptr();
//  test_shared_ptr_for_set();
//  test_comparator();
//  test_loop();
//  test_method_for_map();
//  test_string();
//  test_rand();
//  test_array();
//  test_map();
//  test();

  /*
#pragma omp parallel
  printf("Hello from thread %d, nthreads %d\n",
         omp_get_thread_num(), omp_get_num_threads());

  const char *graph_file = "/Users/esaliya/sali/projects/graphs/data/snap/fascia_com-orkut.ungraph.txt";
  std::ifstream file_g;
  std::string line;
  int* srcs_g;
  int* dsts_g;
  int n_g;
  unsigned m_g;

  int rank = 0;

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  file_g.open(graph_file);

  // Read in labels and vertices for graph
  getline(file_g, line);
  n_g = atoi(line.c_str());
  getline(file_g, line);
  m_g = (unsigned int) strtoul(line.c_str(), NULL, 10);

  srcs_g = new int[m_g];
  dsts_g = new int[m_g];


  for (unsigned i = 0; i < m_g; ++i)
  {
    getline(file_g, line, ' ');
    srcs_g[i] = atoi(line.c_str());
    getline(file_g, line);
    dsts_g[i] = atoi(line.c_str());
  }

  file_g.close();
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cout << "*****Rank: 0 finished reading the whole graph in " << elapsed_seconds.count() << " seconds";

   */

}
