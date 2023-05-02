#!/bin/bash

cwd=$(pwd)
hostnode="b101"
bfdnode="bfd101"

#First compile on host:
echo "Compiling on the host..."
module load gnu_comp/9.3.0 openmpi/4.1.1
mpicxx -O3 -o usrhost usr.cpp
module purge
echo "Compiled on host!"

#Then compile on bfd:
echo "Compiling on the bfd..."
ssh bfd116 "export OMPI_CXX=/usr/bin/g++; cd $cwd; mpicxx -O3 -o usrbfd usr.cpp;"
echo "Compiled on the bfd!"

#Then execute
echo "Executing across host and bfd..."
mpirun -n 1 --host $hostnode usrhost : -n 1 --host $bfdnode --mca pml_ucx_tls 1 usrbfd

echo "Finished!"
