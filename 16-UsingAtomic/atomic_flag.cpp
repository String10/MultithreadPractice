#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <sstream>

std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;
std::stringstream stream;

// using atomic_flag as a lock
void appendNumber(int x) {
    while(lock_stream.test_and_set(std::memory_order_acquire));

    stream << "thread #" << x << '\n';

    lock_stream.clear(std::memory_order_release);
}

int main() {
    std::vector <std::thread> threads;
    for(int i = 0; i < 10; i++) {
        threads.push_back(std::thread(appendNumber, i));
    }
    for(auto &th: threads) {
        th.join();
    }

    std::cout << stream.str();
}