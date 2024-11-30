#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <limits>
#include <fmt/core.h>
#include <mpi.h>


namespace Gauss
{
    using namespace std;

    void debug_root(const std::string& msg, int rank)
    {
        if (rank == 0) {
            std::cout << msg << std::endl;
        }
    }

    void print(vector<vector<double>> &A)
    {
        int n = A.size();
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n + 1; j++)
            {
                cout << A[i][j] << "\t";
                if (j == n - 1)
                {
                    cout << "| ";
                }
            }
            cout << "\n";
        }
        cout << endl;
    }

    vector<double> gauss(vector<vector<double>> &A, int rank, int npi_size)
    {
        int n = A.size();

        // итерируемся по строкам
        for (int i = 0; i < n; ++i)
        {
            // Найти максимум в столбце i (распределенный поиск)
            double localMaxEl = -1 * std::numeric_limits<double>::max();
            int localMaxRow = -1;
            
            // Распределение строк между процессами по индексу с шагом npi_size
            for (int k = i + rank; k < n; k += npi_size)
            {
                debug_root("finding localmax...", rank);
                if (abs(A[k][i]) > localMaxEl)
                {
                    localMaxEl = abs(A[k][i]);
                    localMaxRow = k;
                }
            }

            struct
            {
                double value;
                int row;
            } localMax = {localMaxEl, localMaxRow}, globalMax;

            // Ищем максимальный элемент в столбце всеми процессами
            debug_root("all reduce...", rank);
            MPI_Allreduce(&localMax, &globalMax, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);

            int maxRow = globalMax.row;
            debug_root(fmt::format("maxRow = {}, maxValue = {}", maxRow, globalMax.value), rank);
            if (rank == 0 && maxRow != i)
            {
                // Обмен строк в главном процессе
                for (int k = i; k < n + 1; ++k)
                {
                    swap(A[maxRow][k], A[i][k]);
                    if (rank == 0) {
                        cout << "Current result:\n";
                        print(A);
                    }
                }
            }
            debug_root("swapped elements...", rank);

            

            // Разослать обновленную матрицу всем процессам
            MPI_Bcast(&A[i][0], n + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

             // Обнулить строки ниже текущей с использованием MPI_Send и MPI_Recv
            for (int k = i + 1; k < n; ++k)
            {
                if (rank == (k % npi_size)) // Каждый процесс обрабатывает только свои строки
                {   
                    if (A[i][i] == 0) {
                        continue;
                    }
                    double c = -A[k][i] / A[i][i];
                    for (int j = i; j < n + 1; ++j)
                    {
                        if (i == j)
                        {
                            A[k][j] = 0;
                        }
                        else
                        {
                            A[k][j] += c * A[i][j];
                        }
                    }
                    // Отправить обновленную строку процессу 0
                    if (rank != 0) {
                        MPI_Send(&A[k][0], n + 1, MPI_DOUBLE, 0, k, MPI_COMM_WORLD);
                    }
                }
                else if (rank == 0) // Процесс 0 собирает обновления
                {   
                    debug_root("receiving line...", rank);
                    MPI_Recv(&A[k][0], n + 1, MPI_DOUBLE, k % npi_size, k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (rank == 0) {
                        cout << "Current result:\n";
                        print(A);
                    }
                }
            }

            // Собрать матрицу с обнуленными строками
            for (int k = i + 1; k < n; ++k)
            {
                debug_root("broadcastring result...", rank);
                MPI_Bcast(&A[k][0], n + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            }

            if (rank == 0) {
                cout << "Current result:\n";
                print(A);
            }
        }

        // Решить уравнение Ax = b
        vector<double> x(n);
        if (rank == 0)
        {
            debug_root("calculating result...", rank);
            for (int i = n - 1; i >= 0; i--)
            {
                x[i] = A[i][n] / A[i][i];
                for (int k = i - 1; k >= 0; k--)
                {
                    A[k][n] -= A[k][i] * x[i];
                }
            }
            debug_root("result calculated...", rank);
        }

        if (rank == 0) {
                cout << "Current result:\n";
                print(A);
        }

        // TODO: Понять нужно ли это???
        // Разослать результат решения
        // MPI_Bcast(&x[0], n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        return x;
    }
}

int main(int argc, char **argv)
{
    using namespace std;
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    vector<vector<double>> A;

    if (rank == 0)
    {
        cin >> n;
        A.resize(n, vector<double>(n + 1, 0));

        // Ввод данных
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cin >> A[i][j];
            }
        }
        for (int i = 0; i < n; i++)
        {
            cin >> A[i][n];
        }

        // Печать начальной матрицы
        Gauss::print(A);
    }

    // Разослать рутом размер матрицы всем остальным процессам
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        A.resize(n, vector<double>(n + 1, 0));
    }

    // Разослать матрицу рутом всем остальным процессам
    for (int i = 0; i < n; ++i)
    {
        MPI_Bcast(&A[i][0], n + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Вычисление решения
    vector<double> x = Gauss::gauss(A, rank, size);

    // Печать результата
    if (rank == 0)
    {
        cout << "Result:\t";
        for (int i = 0; i < n; i++)
        {
            cout << x[i] << " ";
        }
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}
