#include <iostream>
#include <atomic>
#include <thread>

std::atomic <int> data(0);
std::atomic <bool> sync1(false), sync2(false);

void thread_1() {
    data.store(114514, std::memory_order_relaxed);

    std::cout << "AAAAAAAAAAAAAA" << std::endl;

    sync1.store(true, std::memory_order_release);
}

void thread_2() {
    while(!sync1.load(std::memory_order_acquire));

    std::cout << 2 << std::endl;

    sync2.store(true, std::memory_order_release);
}

void thread_3() {
    while(!sync2.load(std::memory_order_acquire));

    std::cout << data.load(std::memory_order_relaxed) << std::endl;
}

int main() {
    std::thread th_1(thread_1), th_2(thread_2), th_3(thread_3);
    th_1.join(), th_2.join(), th_3.join();
}