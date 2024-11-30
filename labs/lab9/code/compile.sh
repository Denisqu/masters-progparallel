#!/bin/bash

mpic++ main.cpp -g -o election
#mpirun -n 12 ./election

if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi

