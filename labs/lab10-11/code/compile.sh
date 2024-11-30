#!/bin/bash

mpic++ main.cpp -g -o lab10_11_v1
mpic++ main_v2.cpp -g -o lab10_11_v2
#mpirun -n 12 ./election

if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi

