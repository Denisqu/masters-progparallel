#!/bin/bash

#gcc main.cpp -lstdc++ -lm -Wall -O2 -lgmp -fopenmp -o my_program
#mpic++ main.cpp -lstdc++ -lm -Wall -O2 -o lab12
#mpic++ main_v1.cpp -lstdc++ -lm -Wall -O2 -o lab12_v1
#mpic++ main_v2.cpp -lstdc++ -lm -Wall -O2 -o lab12_v2
#mpic++ main_v3.cpp -lstdc++ -lfmt -lm -Wall -O2 -o lab12_v3
mpic++ main_v4_multi_thread.cpp -lstdc++ -lfmt -lm -Wall -O2 -o lab12_v4_multi

if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi
