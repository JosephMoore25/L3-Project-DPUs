#!/bin/bash

module purge
case "$HOSTNAME" in
    "bluefield"* )
        source /etc/profile.d/modules.sh
        #export OMP_NUM_THREADS=1
        #echo "ON BFD"
        module load /cosma/local/bluefield/workshop/modules/modulefiles/workshop.aarch64
        #module list
        ./$1 --timeout 300
        ;;
    "b1"* )
        #export OMP_NUM_THREADS=1
        module load /cosma/local/bluefield/workshop/modules/modulefiles/workshop.x86_64
        ./$1 --timeout 300
        #module list
        #echo "ON HOST"
        ;;
esac
