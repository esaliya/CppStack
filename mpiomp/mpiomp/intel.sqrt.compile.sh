#!/bin/bash
mpicxx -cxx=icpc -qopenmp -std=c++0x  main.cpp -o intel.sqrt
