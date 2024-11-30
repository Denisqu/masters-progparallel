#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Получение ранга текущего процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Получение общего числа процессов

    const auto hasWinner = [](const std::vector<int>& vec) -> bool {
        if (vec.empty()) {
            return false;
        }
        int maxValue = *std::max_element(vec.begin(), vec.end());
        int count = std::count(vec.begin(), vec.end(), maxValue);
        return count == 1;
    };

    int voting_circles = 0;
    std::vector<int> received_votes {}; // Счётчик голосов, полученных за каждого кандидата
    do {
        ++voting_circles;
        received_votes.clear();
        received_votes.resize(size);
        std::srand(std::time(nullptr) + rank); // Инициализация генератора случайных чисел
        int vote = std::rand() % size; // Голос за кандидата с рангом от 0 до size-1
        int myVotePos = rank; // Каждый процессор голосует в определенный момент времени, а в остальное время слушает
        int votesReceived = 0;

        // Слушаем других пока не время голосовать
        for (int i = 0; i < myVotePos; ++i) {
            int received_vote;
            MPI_Recv(&received_vote, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            ++votesReceived;
            ++received_votes[received_vote];
        }

        // Голосуем
        // Каждый магистр отправляет свой голос всем остальным магистрам
        for (int i = 0; i < size; ++i) {
            if (i != rank) { // Не отправлять голос самому себе
                MPI_Send(&vote, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            else {
                ++received_votes[vote];
                ++votesReceived;
            }
        }
        std::cout << "Магистр " << rank << " проголосовал за номинанта под номером " << vote << std::endl;

        // Дослушиваем остальных
        for (int i = myVotePos + 1; i < size; ++i) {
            int received_vote;
            MPI_Recv(&received_vote, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            ++received_votes[received_vote];
            ++votesReceived;
        }
        
        // Итог круга голосований
        if (rank == 0) {
            std::cout << "\nИтоги голосования (по процессу 0). Всего голосов: " << votesReceived << std::endl;
            for (int i = 0; i < size; ++i) {
                std::cout << "Кандидат " << i << " получил " << received_votes[i] << " голосов.\n";
            }
        }
    } while (!hasWinner(received_votes));

    if (rank == 0) {
        std::cout << "За " << voting_circles << " кругов голосования победил кандидат под номером "
                  << std::distance(received_votes.begin(), std::max_element(received_votes.begin(), received_votes.end()))
                  << std::endl;
    }
    
    MPI_Finalize(); // Завершение MPI
    return 0;
}
