#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::mutex mtx;
int count = 0;

void counter() {
    // 如果不加锁的话就会出现打印乱序的情况
    // 比如
    // count == 1
    // count == 3
    // count == 4
    // count == 5
    // count == 2
    // 甚至如
    // count == count == 21
    // count == 3

    // count == 4
    // count == 5
    // 加锁后其他使用此变量的线程会被阻塞，解锁后再执行

    // 上锁解锁的操作也可以用lock_guard实现，构造时上锁，析构时解锁
    std::lock_guard <std::mutex> lck(mtx);

    // mtx.lock();

    int i = ++count;
    std::cout << "count == " << i << std::endl;

    // mtx.unlock();
}

int main() {
    std::vector <std::thread> threads;
    for(int i = 0; i < 5; i++) {
        threads.push_back(std::thread{ counter });
    }

    for(auto &t: threads) {
        t.join();
    }
}