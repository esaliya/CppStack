#!/bin/bash
hosts=juliet.nodes.txt
nodes=1
ppn=$2
tpp=$1

itr=10
rowperunit=1000
colcount=10000

if [ $tpp -eq 1 ]; then
    echo procs itr $itr rowpu $rowperunit cols $colcount
mpirun -f $hosts -bind-to core:1 -np $(($ppn*$nodes)) ./juliet.intel.mm.exe $tpp $itr $rowperunit $colcount
else 
    echo threads  itr $itr rowpu $rowperunit cols $colcount
    mpirun -f $hosts -bind-to board:1 -np $(($ppn*$nodes)) ./juliet.intel.mm.exe $tpp $itr $rowperunit $colcount

fi
