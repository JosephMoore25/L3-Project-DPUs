#!/bin/bash

module purge
module load gnu_comp/9.3.0 openmpi/4.1.1

mpicxx -O3 -o pollhost poll.cpp

ssh bfd116 "export OMPI_CXX=/usr/bin/g++; cd bfd_tests/Polling; mpicxx -O3 -o pollbfd poll.cpp;"
