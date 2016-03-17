#!/bin/bash
mpicxx -cxx=icpc -qopenmp -std=c++0x -O3 letomppar.cpp -o juliet.intel.mm.exe
