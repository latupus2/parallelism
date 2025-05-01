#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

void initialize_row(vector<double>& matrix_row, double& vector_element, int size, int i) {
    vector_element = i + 1;
    for (int j = 0; j < size; ++j) {
        matrix_row[j] = (i == j) ? 2.0 : 1.0;
    }
}

void initialize_matrix_and_vector(vector<vector<double>>& matrix, vector<double>& vector, int size, int num_threads) {
    std::vector<thread> threads;
    int rows_per_thread = size / num_threads;
    
    for (int t = 0; t < num_threads; ++t) {
        int start = t * rows_per_thread;
        int end = (t == num_threads - 1) ? size : (t + 1) * rows_per_thread;
        
        threads.emplace_back([&matrix, &vector, size, start, end]() {
            for (int i = start; i < end; ++i) {
                initialize_row(ref(matrix[i]), ref(vector[i]), size, i);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

void multiply_row(const vector<vector<double>>& matrix, const vector<double>& vector, std::vector<double>& result, int size, int start, int end) {
    for (int i = start; i < end; ++i) {
        for (int j = 0; j < size; ++j) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
}

vector<double> matrix_vector_multiply(const vector<vector<double>>& matrix, const vector<double>& vector, int size, int num_threads) {
    std::vector<double> result(size, 0.0);
    std::vector<thread> threads;
    int rows_per_thread = size / num_threads;
    
    for (int t = 0; t < num_threads; ++t) {
        int start = t * rows_per_thread;
        int end = (t == num_threads - 1) ? size : (t + 1) * rows_per_thread;
        
        threads.emplace_back(multiply_row, cref(matrix), cref(vector), ref(result), size, start, end);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return result;
}

int main() {
    const int sizes[] = {20000, 40000};
    const int threads_counts[] = {1, 2, 4, 7, 8, 16, 20, 40};
    
    for (int size : sizes) {
        cout << "Matrix size: " << size << "x" << size << endl;
        
        vector<vector<double>> matrix(size, vector<double>(size));
        vector<double> vector(size);
        
        for (int num_threads : threads_counts) {
            cout << "Threads: " << num_threads << flush;
            
            auto start_init = chrono::high_resolution_clock::now();
            initialize_matrix_and_vector(matrix, vector, size, num_threads);
            auto end_init = chrono::high_resolution_clock::now();
            
            auto start_mult = chrono::high_resolution_clock::now();
            auto result = matrix_vector_multiply(matrix, vector, size, num_threads);
            auto end_mult = chrono::high_resolution_clock::now();
            
            auto init_time = chrono::duration_cast<chrono::milliseconds>(end_init - start_init).count();
            auto mult_time = chrono::duration_cast<chrono::milliseconds>(end_mult - start_mult).count();
            
            cout << " | Init time: " << init_time << " ms"
                 << " | Mult time: " << mult_time << " ms" << endl;
        }
    }
    
    return 0;
}