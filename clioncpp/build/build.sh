#!/bin/bash
cmake -DCMAKE_CXX_COMPILER=/cm/shared/apps/mpich/ge/gcc/64/3.1/bin/mpicxx -DCMAKE_C_COMPILER=/cm/shared/apps/mpich/ge/gcc/64/3.1/bin/mpicc -DBOOST_ROOT=/home/esaliya/sali/software/boost_1_63_0 -DBOOST_INCLUDEDIR=/home/esaliya/sali/software/builds/boost/include -DBOOST_LIBRARYDIR=/home/esaliya/sali/software/builds/boost/lib ..
