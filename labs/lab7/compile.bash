#!/bin/bash

#gcc main.cpp -lstdc++ -lm -Wall -O2 -lgmp -fopenmp -o my_program
gcc main.cpp -lstdc++ -lm -Wall -O2 -lgmp -o my_program
gcc main_pthread.cpp -lstdc++ -lm -Wall -O2 -lgmp -o my_program_pthread

if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi
