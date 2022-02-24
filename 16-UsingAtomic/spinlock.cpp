#include <iostream>
#include <atomic>
#include <thread>
#include <sstream>
#include <vector>

class spinlock_mutex {
    //标准原子信号
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    spinlock_mutex() {}
    void lock() {
        // 等待释放
        while(flag.test_and_set(std::memory_order_acquire));
    }
    void unlock() {
        //清除释放数据
        flag.clear(std::memory_order_release);
    }
};

spinlock_mutex mtx;
std::stringstream stream;

void appendNumber(int x) {
    mtx.lock();

    stream << "thread #" << x << '\n';

    mtx.unlock();
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