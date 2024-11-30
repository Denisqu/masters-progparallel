#include <iostream>
#include <cmath>
#include <vector>
#include <mpi.h>

namespace Gauss {

using namespace std;

void print(vector< vector<double> > A) {
    int n = A.size();
    for (int i=0; i<n; i++) {
        for (int j=0; j<n+1; j++) {
            cout << A[i][j] << "\t";
            if (j == n-1) {
                cout << "| ";
            }
        }
        cout << "\n";
    }
    cout << endl;
}

vector<double> gauss(vector< vector<double> >& A) {
    int n = A.size();

    for (int i=0; i<n; i++) {
        // Search for maximum in this column

        // TODO: Это можно распараллелить
        double maxEl = abs(A[i][i]);
        int maxRow = i;
        for (int k=i+1; k<n; k++) {
            if (abs(A[k][i]) > maxEl) {
                maxEl = A[k][i];
                maxRow = k;
            }
        }

        // Swap maximum row with current row (column by column)
        // Здесь мы должны в главном потоке дождаться результатов от других потоков, которые делали работу по поиску максимума в колонке
        for (int k=i; k<n+1;k++) {
            double tmp = A[maxRow][k];
            A[maxRow][k] = A[i][k];
            A[i][k] = tmp;
        }

        // Make all rows below this one 0 in current column
        // TODO: Это можно распараллелить
        for (int k=i+1; k<n; k++) {
            double c = -A[k][i]/A[i][i];
            for (int j=i; j<n+1; j++) {
                if (i==j) {
                    A[k][j] = 0;
                } else {
                    A[k][j] += c * A[i][j];
                }
            }
        }
    }

    // Solve equation Ax=b for an upper triangular matrix A
    vector<double> x(n);
    for (int i=n-1; i>=0; i--) {
        x[i] = A[i][n]/A[i][i];
        for (int k=i-1;k>=0; k--) {
            A[k][n] -= A[k][i] * x[i];
        }
    }
    return x;
}

}

int main() {
    int n;
    std::cin >> n;

    std::vector<double> line(n+1,0);
    std::vector<std::vector<double>> A(n,line);

    // Read input data
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            std::cin >> A[i][j];
        }
    }

    for (int i=0; i<n; i++) {
        std::cin >> A[i][n];
    }

    // Print input
    Gauss::print(A);

    // Calculate solution
    std::vector<double> x(n);
    x = Gauss::gauss(A);

    // Print result
    std::cout << "Result:\t";
    for (int i=0; i<n; i++) {
        std::cout << x[i] << " ";
    }
    std::cout << std::endl;
}