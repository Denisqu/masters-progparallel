#!/bin/bash

# Указываем имя исходного файла и выходного исполняемого файла
SOURCE_FILE="main.cpp ./libs/Simple-FFT/unit-tests/test_fft.cpp"
OUTPUT_FILE="app"

benchmark() {
    local compile_flags=$1
    local description=$2
    
    echo "========================================"
    echo "Компиляция с опциями: $description"
    
    g++ $SOURCE_FILE -o $OUTPUT_FILE $compile_flags -std=c++17 -lstdc++ -lm
    
    if [[ $? -ne 0 ]]; then
        echo "Ошибка компиляции с опциями: $description"
        return
    fi

    echo "Запуск программы с опциями: $description"
    time ./$OUTPUT_FILE
    echo ""
}

benchmark "-O2 -fipa-pta -fipa-cp -fipa-cp-clone -flto" "-O2 с межпроцедурной оптимизацией и оптимизацией времени компоновки (-fipa-*, -flto)"

echo "Генерация профиля для -O2 с -fprofile-generate..."
g++ $SOURCE_FILE -o $OUTPUT_FILE -O2 -fprofile-generate -std=c++17 -lstdc++ -lm
./$OUTPUT_FILE

benchmark "-O2 -fprofile-use" "-O2 с профилируемой оптимизацией с обратной связью (-fprofile-use)"

echo "Генерация профиля для -O2 с -fipa-*, -flto, -fprofile-generate..."
g++ $SOURCE_FILE -o $OUTPUT_FILE -O2 -fipa-pta -fipa-cp -fipa-cp-clone -flto -fprofile-generate -std=c++17 -lstdc++ -lm
./$OUTPUT_FILE

benchmark "-O2 -fipa-pta -fipa-cp -fipa-cp-clone -flto -fprofile-use" "-O2 с межпроцедурной оптимизацией, оптимизацией компоновки и оптимизацией с обратной связью (-fipa-*, -flto, -fprofile-use)"

echo "Бенчмарк завершен."