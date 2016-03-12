#!/bin/bash
hosts=nodes.txt
nodes=2
ppn=$2
tpp=$1

itr=1
rowperunit=1000
colcount=10000

if [ $tpp -eq 1 ]; then
    echo procs itr $itr rowpu $rowperunit cols $colcount
/N/u/sekanaya/fg-buildompi-1.10.2/bin/mpirun  --report-bindings -hostfile $hosts --map-by ppr:$ppn:node -bind-to core -np $(($ppn*$nodes)) ./fg.ompi.mm $tpp $itr $rowperunit $colcount
else 
    echo threads  itr $itr rowpu $rowperunit cols $colcount
/N/u/sekanaya/fg-buildompi-1.10.2/bin/mpirun  --report-bindings --map-by ppr:1:node:PE=8,SPAN -bind-to none -hostfile $hosts -np $(($ppn*$nodes)) ./fg.ompi.mm $tpp $itr $rowperunit $colcount
fi
