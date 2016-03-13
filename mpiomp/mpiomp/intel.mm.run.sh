#!/bin/bash
hosts=nodes.txt
nodes=4
ppn=$2
tpp=$1

itr=1
rowperunit=1000
colcount=10000

if [ $tpp -eq 1 ]; then
    echo procs itr $itr rowpu $rowperunit cols $colcount
mpirun -f $hosts -bind-to core:1 -np $(($ppn*$nodes)) ./intel.mm $tpp $itr $rowperunit $colcount
else 
    echo threads  itr $itr rowpu $rowperunit cols $colcount
    mpirun -f $hosts -bind-to board:1 -np $(($ppn*$nodes)) ./intel.mm $tpp $itr $rowperunit $colcount

fi
