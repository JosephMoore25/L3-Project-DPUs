#!/bin/bash

module purge
module load /cosma/local/bluefield/workshop/modules/modulefiles/workshop.x86_64
cd ../build/x86_64

mpirun -n 4 -H b101,b102,b103,b104 ./baseline | tee ../../data/baselinedata.txt

ssh bfd116 "bash -lc 'module load /cosma/local/bluefield/workshop/modules/modulefiles/workshop.aarch64; cd ~/L3-Project-DPUs/build/aarch64; mpirun -n 4 -H bluefield101,bluefield102,bluefield103,bluefield104 ./dpu-miniappbfd | tee ../../data/demobfddata.txt'"

cd ../../Graphing-Code
module load python/3.10.7
python3 graph_normal.py