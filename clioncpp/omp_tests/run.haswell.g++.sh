#!/bin/bash
export OMP_NUM_THREADS=1
host=`hostname`
cd /home/esaliya/sali/git/github/esaliya/ccpp/CppStack/clioncpp/omp_tests
./omp.haswell.g++ > haswell.outs/"$host"."$OMP_NUM_THREADS".txt
