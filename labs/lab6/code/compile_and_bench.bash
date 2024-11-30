#!/bin/bash

#gcc main.cpp -lstdc++ -lm -Wall -O2 -fopenmp -o my_program
#gcc main.cpp -lstdc++ -lm -Wall -O2 -o my_program
gcc main_pthread.cpp -lstdc++ -lm -Wall -O2 -o my_program_pthread

if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi

echo "Running program..."
EXECUTION_TIME=$(time ./my_program) #2>&1 | grep real | awk '{print $2}')


echo "$EXECUTION_TIME"
echo "---------------------------------"