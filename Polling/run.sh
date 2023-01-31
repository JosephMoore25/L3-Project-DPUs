#!/bin/bash

mpirun -n 1 --host bfh101 pollhost : -n 1 --host bfd101 --mca pml_ucx_tls 1 pollbfd
