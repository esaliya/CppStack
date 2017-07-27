#include <omp.h>
#include <stdio.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <math.h>

typedef std::chrono::duration<double, std::milli> ms_t;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> ticks_t;

typedef std::chrono::high_resolution_clock hrc_t;

void openmp_parfor_test(){
  int size = 16777216;
  std::vector<double> *vec = new std::vector<double>((unsigned long) size);

  ticks_t start_ticks = hrc_t::now();

//  omp_set_num_threads(1);
#pragma omp parallel
{
  int num_threads = omp_get_num_threads();
  std::cout<<"num threads: "<<num_threads;
#pragma omp for
  for (int i = 0; i < size; ++i) {
    (*vec)[i] = sqrt(i);
  }
}

  ticks_t end_ticks = hrc_t::now();

  std::cout<<"Loop initialization took (ms) "<<(ms_t(end_ticks - start_ticks).count())<<std::endl;

  delete vec;

}

int main() {
  openmp_parfor_test();
  return 0;
}
