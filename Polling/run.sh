#!/bin/bash

mpirun -n 1 --host b118 pollhost : -n 1 --host bfd118 --mca pml_ucx_tls 1 pollbfd
