#include <iostream>
#include <cmath>
#include <chrono>
#include <omp.h>

using namespace std;

double f(double x) {
    return sin(x);
}

double integrate_omp(double a, double b, int nsteps) {
    double h = (b - a) / nsteps;
    double sum = 0.0;
    
    #pragma omp parallel
    {
        double local_sum = 0.0;
        
        #pragma omp for
        for (int i = 0; i < nsteps; ++i) {
            double x = a + (i + 0.5) * h;
            local_sum += f(x);
        }
        
        #pragma omp atomic
        sum += local_sum;
    }
    
    return sum * h;
}

int main() {
    const double a = 0.0;
    const double b = M_PI;
    const int nsteps = 40000000;
    const int threads[] = {1, 2, 4, 7, 8, 16, 20, 40};
    
    for (int num_threads : threads) {
        omp_set_num_threads(num_threads);
        
        auto start = chrono::high_resolution_clock::now();
        double result = integrate_omp(a, b, nsteps);
        auto end = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        
        cout << "Threads: " << num_threads 
                  << " | Result: " << result 
                  << " | Time: " << duration << " ms" << std::endl;
    }
    
    return 0;
}