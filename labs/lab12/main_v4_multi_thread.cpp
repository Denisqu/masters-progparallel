#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <limits>
#include <algorithm> 
#include <fmt/core.h>
#include <tuple>
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

    void root_printMatrix(vector<vector<double>> &A, int rank)
    {
        if (rank != 0) {
            return;
        }

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

    std::vector<double> gauss(vector<vector<double>> &A, int rank, int npi_size)
    {
        const auto rowsCount             = A.size();
        const auto variablesColumnsCount = A[0].size() - 1;
        const auto totalColumnsCount     = A[0].size();
        debug_root(fmt::format("rowsCount = {}, variablesColumnsCount = {}, totalColumnsCount = {}",
                    rowsCount, variablesColumnsCount, totalColumnsCount), rank);

        const auto findRowWithMaxColumn = [&A, &rowsCount, &variablesColumnsCount, &rank, &npi_size]
                                          (int column, int startRow) -> std::tuple<int, double> {
            auto startIndex = startRow + rank;
            auto max = startIndex < rowsCount ? std::abs(A[startIndex][column]) : -1;
            auto index = startIndex < rowsCount ? startIndex : -1;
            // Распределение строк между процессами по индексу с шагом npi_size
            if (startIndex < rowsCount) {
                for (size_t row = startRow + rank; row < rowsCount; row += npi_size) {
                    if (const auto value = std::abs(A[row][column]); value > max) {
                        max = value;
                        index = row;
                        debug_root(fmt::format("New max: {}, index: {}", max, index), rank);
                    }
                }
            }
            struct
            {
                double value;
                int row;
            } localMax = {max, index}, globalMax;
            // Ищем максимальный элемент в столбце всеми процессами
            debug_root("all reduce...", rank);
            MPI_Allreduce(&localMax, &globalMax, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
            return { globalMax.row, globalMax.value };
        };
        const auto swapRows = [&A, &totalColumnsCount, &rank] (int firstRow, int secondRow) {
            if (rank == 0) {
                for (size_t i = 0; i < totalColumnsCount; ++i) {
                    std::swap(A[firstRow][i], A[secondRow][i]);
                } 
            }
            // Разослать обновленную матрицу всем процессам
            MPI_Bcast(&A[firstRow][0], totalColumnsCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Bcast(&A[secondRow][0], totalColumnsCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        };
        const auto normalizeMatrix = [&A, &totalColumnsCount, &rowsCount, &rank, &npi_size] (int normalizedColumn) {
            // нормализуем свои строки
            for (size_t row = rank; row < rowsCount; row += npi_size) {
                const auto factor = A[row][normalizedColumn];
                for (size_t column = 0; column < totalColumnsCount; ++column) {
                    A[row][column] = A[row][column] / factor;
                }
            }
            // синхронизируем состояние между всеми процессами
            if (rank != 0) {
                for (size_t row = rank; row < rowsCount; row += npi_size) {
                    MPI_Send(&A[row][0], totalColumnsCount, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
                }
            } 
            else {
                // слушаем чужие строки
                for (auto syncedRank = 1; syncedRank < npi_size; ++syncedRank) {
                    for (size_t row = syncedRank; row < rowsCount; row += npi_size) {
                        MPI_Recv(&A[row][0], totalColumnsCount, MPI_DOUBLE,
                                 syncedRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        debug_root(fmt::format("received row A[{}][{}]:", row, 0), rank);
                        root_printMatrix(A, rank);
                    }
                }      
            }
            // отправляем всем итоговую матрицу
            for (size_t row = 0; row < rowsCount; ++row) {
                MPI_Bcast(&A[row][0], totalColumnsCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            }  
        };
        const auto subtractRows = [&A, &totalColumnsCount, &rank, &npi_size, &rowsCount] (std::vector<int> reducedRows, int subtractedRow) {
            // вычитаем свои строки
            for (size_t i = rank; i < reducedRows.size(); i += npi_size) {
                for (size_t column = 0; column < totalColumnsCount; ++column) {
                    A[reducedRows[i]][column] = A[reducedRows[i]][column] - A[subtractedRow][column];
                }
            }
            // синхронизируем состояние между всеми процессами
            if (rank != 0) {
                for (size_t i = rank; i < reducedRows.size(); i += npi_size) {
                    MPI_Send(&A[reducedRows[i]][0], totalColumnsCount, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
                }
            } 
            else {
                // слушаем чужие строки
                for (auto syncedRank = 1; syncedRank < npi_size; ++syncedRank) {
                    for (size_t i = syncedRank; i < reducedRows.size(); i += npi_size) {
                        MPI_Recv(&A[reducedRows[i]][0], totalColumnsCount, MPI_DOUBLE,
                                 syncedRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        debug_root(fmt::format("received row A[{}][{}]:", reducedRows[i], 0), rank);
                        root_printMatrix(A, rank);
                    }
                }               
            }
            // отправляем всем итоговую матрицу
            for (size_t row = 0; row < rowsCount; ++row) {
                debug_root("broadcasting matrix after subtraction...", rank);
                MPI_Bcast(&A[row][0], totalColumnsCount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            } 

        };
        const auto solveEquations = [&A, &totalColumnsCount, &rowsCount] () -> std::vector<double> {
            std::vector<double> result(rowsCount, 0);
            for (int i = rowsCount - 1; i >= 0; i--) {
                result[i] = A[i][rowsCount] / A[i][i];
                for (int k = i - 1; k >= 0; k--) {
                    A[k][rowsCount] -= A[k][i] * result[i];
                }
            }
            return result;
        };

        // main loop
        for (size_t iteration = 0; iteration < rowsCount; ++iteration) {
            const auto [maxRow, max] = findRowWithMaxColumn(iteration, iteration);
            debug_root(fmt::format("iteration = {}, maxRow = {}, max = {}. Matrix after swap:",
                                    iteration, maxRow, max), rank);
                                    
            swapRows(iteration, maxRow);
            root_printMatrix(A, rank);

            normalizeMatrix(iteration);
            debug_root("Matrix after normalization:", rank);
            root_printMatrix(A, rank);
            
            std::vector<int> reducedRows;
            for (size_t reducedRow = iteration + 1; reducedRow < rowsCount; ++reducedRow) {
                reducedRows.push_back(reducedRow);
            }
            subtractRows(reducedRows, iteration);
            debug_root("Matrix after subtraction:", rank);
            root_printMatrix(A, rank);
        }

        debug_root("Calculating result...", rank);
        const auto result = solveEquations();
        return result;
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
        Gauss::root_printMatrix(A, rank);
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
        for (size_t i = 0; i < x.size(); i++)
        {
            cout << x[i] << " ";
        }
        cout << endl;
    }

    MPI_Finalize();
    return 0;
}
