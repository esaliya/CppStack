#!/bin/bash
mpicxx -cxx=icpc -qopenmp -std=c++0x -O3 main.cpp -o intel.sqrt
