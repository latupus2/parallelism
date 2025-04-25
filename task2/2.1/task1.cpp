#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;

void initialize_matrix_and_vector(vector<vector<double>>& matrix, vector<double>& vector, int size) {
    #pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        vector[i] = i + 1;
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = (i == j) ? 2.0 : 1.0;
        }
    }
}

vector<double> matrix_vector_multiply(const vector<vector<double>>& matrix, const vector<double>& vector, int size) {
    std::vector<double> result(size, 0.0);
    
    #pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
    
    return result;
}

int main() {
    const int sizes[] = {20000, 40000};
    const int threads[] = {1, 2, 4, 7, 8, 16, 20, 40};
    
    for (int size : sizes) {
        cout << "Matrix size: " << size << "x" << size << endl;
        
        vector<vector<double>> matrix(size, vector<double>(size));
        vector<double> vector(size);
        
        for (int num_threads : threads) {
            omp_set_num_threads(num_threads);
            
            auto start_init = chrono::high_resolution_clock::now();
            initialize_matrix_and_vector(matrix, vector, size);
            auto end_init = chrono::high_resolution_clock::now();
            
            auto start_mult = chrono::high_resolution_clock::now();
            auto result = matrix_vector_multiply(matrix, vector, size);
            auto end_mult = chrono::high_resolution_clock::now();
            
            auto init_time = chrono::duration_cast<chrono::milliseconds>(end_init - start_init).count();
            auto mult_time = chrono::duration_cast<chrono::milliseconds>(end_mult - start_mult).count();
            
            cout << "Threads: " << num_threads 
                      << " | Init time: " << init_time << " ms"
                      << " | Mult time: " << mult_time << " ms" << endl;
        }
    }
    
    return 0;
}