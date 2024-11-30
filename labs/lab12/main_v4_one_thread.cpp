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

    void printMatrix(vector<vector<double>> &A)
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

    std::vector<double> gauss(vector<vector<double>> &A, int rank, int npi_size)
    {
        const auto rowsCount             = A.size();
        const auto variablesColumnsCount = A[0].size() - 1;
        const auto totalColumnsCount     = A[0].size();
        debug_root(fmt::format("rowsCount = {}, variablesColumnsCount = {}, totalColumnsCount = {}",
                    rowsCount, variablesColumnsCount, totalColumnsCount), rank);

        const auto findRowWithMaxColumn = [&A, &rowsCount, &variablesColumnsCount, &rank]
                                          (int column, int startRow) -> std::tuple<int, double> {
            auto max = std::abs(A[startRow][column]);
            auto index = startRow;
            for (size_t row = startRow; row < rowsCount; ++row) {
                if (const auto value = std::abs(A[row][column]); value > max) {
                    max = value;
                    index = row;
                    debug_root(fmt::format("New max: {}, index: {}", max, index), rank);
                }
            }
            return { index, max };
        };
        const auto swapRows = [&A, &totalColumnsCount] (int firstRow, int secondRow) {
            for (size_t i = 0; i < totalColumnsCount; ++i) {
                std::swap(A[firstRow][i], A[secondRow][i]);
            }
        };
        const auto normalizeMatrix = [&A, &totalColumnsCount, &rowsCount] (int normalizedColumn) {
            for (size_t row = 0; row < rowsCount; ++row) {
                const auto factor = A[row][normalizedColumn];
                for (size_t column = 0; column < totalColumnsCount; ++column) {
                    A[row][column] = A[row][column] / factor;
                }
            }
        };
        const auto subtractRows = [&A, &totalColumnsCount] (int reducedRow, int subtractedRow) {
            for (size_t column = 0; column < totalColumnsCount; ++column) {
                A[reducedRow][column] = A[reducedRow][column] - A[subtractedRow][column];
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
            printMatrix(A);
            normalizeMatrix(iteration);
            debug_root("Matrix after normalization:", rank);
            printMatrix(A);
            
            for (size_t reducedRow = iteration + 1; reducedRow < rowsCount; ++reducedRow) {
                subtractRows(reducedRow, iteration);
            }
            debug_root("Matrix after subtraction:", rank);
            printMatrix(A);
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
        Gauss::printMatrix(A);
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
