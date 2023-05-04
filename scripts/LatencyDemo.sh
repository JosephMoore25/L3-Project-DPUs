#!/bin/bash

module purge
module load /cosma/local/bluefield/workshop/modules/modulefiles/workshop.x86_64
cd ../build/x86_64

mpirun -n 4 -H b101,b102,b103,b104 ./latencyhost | tee ../../data/latdemohostdata.txt

mpirun -n 2 -H b101,b102 ./baselinelatency | tee ../../data/latdemobaselinedata.txt

ssh bfd116 "bash -lc 'module load /cosma/local/bluefield/workshop/modules/modulefiles/workshop.aarch64; cd ~/L3-Project-DPUs/build/aarch64; mpirun -n 4 -H bluefield101,bluefield102,bluefield103,bluefield104 ./latencybfd | tee ../../data/latdemobfddata.txt'"

cd ~/L3-Project-DPUs
module load python/3.10.7
python3 Graphing-Code/graph_latency.py data/latdemohostdata.txt data/latdemobfddata.txt data/latdemobaselinedata.txt
mv latdemohostdata.png ./plots