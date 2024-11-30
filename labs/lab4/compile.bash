#!/bin/bash

gcc main.cpp ./libs/Simple-FFT/unit-tests/test_fft.cpp -lstdc++ -lm -Wall -O0 -o my_program_o0
gcc main.cpp ./libs/Simple-FFT/unit-tests/test_fft.cpp -lstdc++ -lm -Wall -O1 -o my_program_o1
gcc main.cpp ./libs/Simple-FFT/unit-tests/test_fft.cpp -lstdc++ -lm -Wall -O2 -o my_program_o2
gcc main.cpp ./libs/Simple-FFT/unit-tests/test_fft.cpp -lstdc++ -lm -Wall -O3 -o my_program_o3
gcc main.cpp ./libs/Simple-FFT/unit-tests/test_fft.cpp -lstdc++ -lm -Wall -O3 -funroll-loops -o my_program_o3_unroll

if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi
