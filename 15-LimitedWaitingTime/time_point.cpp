#include <iostream>
#include <chrono>

int main() {
    // std::chrono::time_point <std::chrono::high_resolution_clock>
    // or
    // std::chrono::high_resolution_clock::time_point
    auto begin = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < 100000; i++);

    // std::chrono::time_point <std::chrono::high_resolution_clock>
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration <double> time_span = std::chrono::duration_cast <std::chrono::duration <double>>(end - begin);

    std::cout << "Loop took "
              << time_span.count()
              << " seconds" << std::endl;
}