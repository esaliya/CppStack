#!/bin/bash
hosts=nodes.txt
nodes=1
ppn=$2
tpp=$1

itr=10
skip=0
load=100

if [ $tpp -eq 1 ]; then
    echo procs itr $itr skip $skip load $load
mpirun -f $hosts -bind-to core:1 -np $(($ppn*$nodes)) ./intel.sqrt $tpp $itr $skip $load
else 
    echo procs itr $itr skip $skip load $load
    mpirun -f $hosts -bind-to board:1 -np $(($ppn*$nodes)) ./intel.sqrt $tpp $itr $skip $load
fi
