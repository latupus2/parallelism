#include <iostream>
#include <cmath>
#include <chrono>

#ifdef USE_DOUBLE
using ArrayType = double;
#else
using ArrayType = float;
#endif

const size_t size = 10000000;
const double pi = 3.14159265358979323846;

int main() {
    ArrayType* array = new ArrayType[size];
    std::cout << "Array type: " << typeid(array[0]).name() << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < size; ++i) {
        array[i] = sin(2 * pi * i / size);
    }

    ArrayType sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += array[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Sum of array elements: " << sum << std::endl;
    std::cout << "Time: " << elapsed.count() << " seconds" << std::endl;

    delete[] array;
    return 0;
}