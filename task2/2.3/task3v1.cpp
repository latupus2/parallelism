#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>

using namespace std;

const double EPSILON = 1e-5;
const double TAU = 1e-6;
const int N = 1000;

using Vector = vector<double>;
using Matrix = vector<Vector>;

double norm(const Vector& v) {
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; i++) {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

Vector simpleIterationMethod(const Matrix& A, const Vector& b) {
    Vector x(N, 0.0), r(N), Ax(N);
    
    while (true) {
        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            double sum = 0.0;
            for (int j = 0; j < N; j++) {
                sum += A[i][j] * x[j];
            }
            Ax[i] = sum;
            r[i] = sum - b[i];
        }
        
        if (norm(r) / norm(b) < EPSILON) break;

        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            x[i] -= TAU * r[i];
        }
    }
    return x;
}

int main() {
    Matrix A(N, Vector(N, 1.0));
    for (int i = 0; i < N; i++) A[i][i] = 2.0;
    Vector b(N, N + 1);

    for (int num_threads : {1, 2, 4, 7, 8, 16, 20, 40}) {
        omp_set_num_threads(num_threads);
        
        double start = omp_get_wtime();
        Vector solution = simpleIterationMethod(A, b);
        double time = omp_get_wtime() - start;
        
        cout << "Threads: " << num_threads 
             << " Time: " << time << " s" << endl;
    }
    
    return 0;
}