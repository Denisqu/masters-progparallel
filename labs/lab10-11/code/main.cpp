#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <mpi.h>

namespace {

void printVector(const std::vector<int>& vec)
{   
    std::cout << "{ ";
    for (const auto& val : vec) {
        std::cout << val << " ";
    }
    std::cout << "}" << std::endl;
}

}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv); // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Получение ранга текущего процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Получение общего числа процессов

    std::vector<int> totalArray;
    // Каждый процесс создает массив чисел от 0 до N (где N - номер процесса)
    std::vector<int> myArray(rank + 1); // Массив из чисел от 0 до rank
    for (int i = 0; i <= rank; ++i) {
        myArray[i] = i;
    }
    totalArray.insert(totalArray.end(), myArray.begin(), myArray.end()); 
    // Слушаем других пока не время отправлять
    for (int i = 0; i < rank; ++i) {
        std::vector<int> received_array(i+1);
        MPI_Recv(received_array.data(), received_array.size(), MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        totalArray.insert(totalArray.end(), received_array.begin(), received_array.end());
    }
    // Отправляем всем кроме себя
    for (int i = 0; i < size; ++i) {
        if (i != rank) {
            MPI_Send(myArray.data(), myArray.size(), MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    // Дослушиваем остальных
    for (int i = rank + 1; i < size; ++i) {
        std::vector<int> received_array(i+1);
        MPI_Recv(received_array.data(), received_array.size(), MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        totalArray.insert(totalArray.end(), received_array.begin(), received_array.end());
    }
    // Вывод результатов
    if (/*rank == 0*/true) {
        std::cout << "Итоговый массив, собранный на процессе " << rank << " ";
        printVector(totalArray);
        std::cout <<"Общая сумма равна " << std::accumulate(totalArray.begin(), totalArray.end(), 0) << std::endl;
    }
    
    MPI_Finalize(); // Завершение MPI
    return 0;
}
