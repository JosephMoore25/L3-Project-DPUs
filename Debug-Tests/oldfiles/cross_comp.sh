#!/bin/bash

module purge
module load gnu_comp/9.3.0 openmpi/4.1.1

mpicxx -O0 -o pollhost poll.cpp
mpicxx -O0 -o onewayhost oneway.cpp

ssh bfd116 "export OMPI_CXX=/usr/bin/g++; cd L3-Project-DPUs/Polling; mpicxx -O0 -o pollbfd poll.cpp; mpicxx -O0 -o onewaybfd oneway.cpp"
