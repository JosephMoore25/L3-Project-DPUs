#!/bin/bash -i

module load /cosma/local/bluefield/workshop/modules/modulefiles/workshop.x86_64
cd ..
make clean
make

ssh bfd116 "cd ~/L3-Project-DPUs; module load /cosma/local/bluefield/workshop/modules/modulefiles/workshop.aarch64; make;"   