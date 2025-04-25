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

Vector simpleIterationMethod(const Matrix &A, const Vector &b, int n_threads, const string& schedule_type, int chunk_size) {
    int n = A.size();
    Vector x(n, 0.0), Ax(n), r(n);
    bool stop = false;

    #pragma omp parallel num_threads(n_threads)
    {
        while (!stop) {
            // Вычисление Ax с выбранным расписанием
            if (schedule_type == "static") {
                #pragma omp for schedule(static, chunk_size)
                for (int i = 0; i < n; ++i) {
                    double sum = 0.0;
                    for (int j = 0; j < n; ++j) {
                        sum += A[i][j] * x[j];
                    }
                    Ax[i] = sum;
                    r[i] = sum - b[i];
                }
            } else if (schedule_type == "dynamic") {
                #pragma omp for schedule(dynamic, chunk_size)
                for (int i = 0; i < n; ++i) {
                    double sum = 0.0;
                    for (int j = 0; j < n; ++j) {
                        sum += A[i][j] * x[j];
                    }
                    Ax[i] = sum;
                    r[i] = sum - b[i];
                }
            } else {
                #pragma omp for schedule(guided, chunk_size)
                for (int i = 0; i < n; ++i) {
                    double sum = 0.0;
                    for (int j = 0; j < n; ++j) {
                        sum += A[i][j] * x[j];
                    }
                    Ax[i] = sum;
                    r[i] = sum - b[i];
                }
            }

            #pragma omp single
            {
                if (norm(r) / norm(b) < EPSILON) {
                    stop = true;
                }
            }

            if (!stop) {
                #pragma omp for schedule(static)
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

    vector<string> schedules = {"static", "dynamic", "guided", "auto"};
    vector<int> chunk_sizes = {1, 4, 8, 16};

    for (int n_threads : {1, 2, 4, 7, 8, 16, 20, 40}) {
        for (const auto& schedule : schedules) {
            for (int chunk_size : chunk_sizes) {
                omp_set_num_threads(n_threads);
                double start = omp_get_wtime();
                Vector solution = simpleIterationMethod(A, b, n_threads, schedule, chunk_size);
                double time = omp_get_wtime() - start;
                
                cout << "Threads: " << n_threads 
                     << " | Schedule: " << schedule 
                     << " | Chunk: " << chunk_size
                     << " | Time: " << time << " s" << endl;
            }
        }
    }

    return 0;
}