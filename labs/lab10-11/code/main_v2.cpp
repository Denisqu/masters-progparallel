#include <iostream>
#include <vector>
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
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Создаем массив чисел от 0 до rank
    std::vector<int> myArray(rank + 1);
    for (int i = 0; i <= rank; ++i) {
        myArray[i] = i;
    }

    /* Количество элементов данных, отправляемых этим процессом в буфер, указанный в параметре sendbuf */
    // Считаем общее количество элементов, отправляемых каждым процессом
    std::vector<int> sendCounts(size, 0);
    for (int i = 0; i < size; ++i) {
        sendCounts[i] = rank + 1;
    }

    // Рассчитываем смещения для отправки
    std::vector<int> sendDisplacements(size, 0);
    for (int i = 1; i < size; ++i) {
        // смещение равно всегда равно нулю т.к каждому процессу отправляем одинаковые данные
        sendDisplacements[i] = 0;
    }

    // Готовим массив для приема данных
    std::vector<int> recvCounts(size);
    std::vector<int> recvDisplacements(size);
    for (int i = 0; i < size; ++i) {
        recvCounts[i] = i + 1;
        recvDisplacements[i] = (i == 0) ? 0 : recvDisplacements[i - 1] + recvCounts[i - 1];
    }
    int totalRecvCount = std::accumulate(recvCounts.begin(), recvCounts.end(), 0);
    std::vector<int> recvBuffer(totalRecvCount);

    // Выполняем обмен данными
    MPI_Alltoallv(
        myArray.data(), sendCounts.data(), sendDisplacements.data(), MPI_INT,
        recvBuffer.data(), recvCounts.data(), recvDisplacements.data(), MPI_INT,
        MPI_COMM_WORLD
    );

    // Вывод результатов
    if (/*rank == 0*/true) {
        std::cout << "Итоговый массив, собранный на процессе " << rank << " ";
        printVector(recvBuffer);
        std::cout << "Общая сумма всех элементов: "
                  << std::accumulate(recvBuffer.begin(), recvBuffer.end(), 0) << std::endl;
    }

    MPI_Finalize();
    return 0;
}
