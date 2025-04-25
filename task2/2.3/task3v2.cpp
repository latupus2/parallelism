#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

using namespace std;

const double EPSILON = 1e-5;
const double TAU = 1e-6;
const int N = 1000; // Фиксированный размер системы

using Vector = vector<double>;
using Matrix = vector<Vector>;

double norm(const Vector &v) {
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < v.size(); ++i) {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

Vector simpleIterationMethod(const Matrix &A, const Vector &b, int n_threads) {
    int n = A.size();
    Vector x(n, 0.0), Ax(n), r(n);
    bool stop = false;

    #pragma omp parallel num_threads(n_threads)
    {
        while (!stop) {
            #pragma omp for
            for (int i = 0; i < n; ++i) {
                double sum = 0.0;
                for (int j = 0; j < n; ++j) {
                    sum += A[i][j] * x[j];
                }
                Ax[i] = sum;
                r[i] = sum - b[i];
            }

            #pragma omp single
            {
                if (norm(r) / norm(b) < EPSILON) {
                    stop = true;
                }
            }

            if (!stop) {
                #pragma omp for
                for (int i = 0; i < n; ++i) {
                    x[i] -= TAU * r[i];
                }
            }
        }
    }
    return x;
}

int main() {
    Matrix A(N, Vector(N, 1.0));
    Vector b(N, N + 1);

    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        A[i][i] = 2.0;
    }

    for (int n_threads : {1, 2, 4, 8, 16, 20, 32, 40, 64, 80}) {
        omp_set_num_threads(n_threads);
        double start = omp_get_wtime();
        Vector solution = simpleIterationMethod(A, b, n_threads);
        double time = omp_get_wtime() - start;
        
        cout << "Threads: " << n_threads 
             << " Time: " << time << " s" << endl;
    }

    return 0;
}