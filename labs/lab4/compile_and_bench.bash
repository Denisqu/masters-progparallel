#!/bin/bash

# Проверяем, что указан исходный файл
if [ -z "$1" ]; then
    echo "Usage: $0 <source_file>"
    exit 1
fi

SOURCE_FILE=$1
OUTPUT_FILE="program_exec"
OPT_FLAGS=(
    "-O0"
    "-Os"
    "-O1"
    "-O2"
    "-O3"
    "-O2 -march=native"
    "-O3 -march=native"
    "-O2 -march=native -funroll-loops"
    "-O3 -march=native -funroll-loops"
)

echo "Starting benchmarks for $SOURCE_FILE..."

# Проходим по всем флагам оптимизации
for OPT in "${OPT_FLAGS[@]}"; do
    # Компилируем программу с текущим флагом оптимизации
    echo "Compiling with optimization flags: $OPT"
    gcc "$SOURCE_FILE" ./libs/Simple-FFT/unit-tests/test_fft.cpp -o "$OUTPUT_FILE" $OPT -lstdc++ -lm

    # Проверяем, успешно ли прошла компиляция
    if [ $? -ne 0 ]; then
        echo "Compilation failed with flags: $OPT"
        continue
    fi

    # Измеряем время выполнения программы
    echo "Running program..."
    EXECUTION_TIME=$( (time ./"$OUTPUT_FILE") 2>&1 | grep real | awk '{print $2}')

    # Измеряем занимаемое программой дисковое пространство
    FILE_SIZE=$(du -b "$OUTPUT_FILE" | cut -f1)

    # Выводим результаты
    echo "Optimization: $OPT"
    echo "Execution Time: $EXECUTION_TIME"
    echo "File Size: $FILE_SIZE bytes"
    echo "---------------------------------"
done

# Удаляем временный исполняемый файл
rm -f "$OUTPUT_FILE"

echo "Benchmarking completed."
