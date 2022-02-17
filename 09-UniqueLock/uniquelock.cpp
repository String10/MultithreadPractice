#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int index = 0;

void printBlock(int n, char c) {
    // unique_lock比lock_guard使用更加灵活，功能更加强大。
    // 使用unique_lock需要付出更多的时间、性能成本。
    // unique_lock有多组构造函数, 这里std::defer_lock不设置锁状态
    std::unique_lock <std::mutex> lck(mtx, std::defer_lock);
    
    // std::lock_guard <std::mutex> lck(mtx);

    std::cout << ++index << std::endl;

    if(lck.try_lock()) {
        for(int i = 0; i < 50; i++) {
            std::cout << c;
        }
        std::cout << std::endl;
    }
}

int main() {
    std::thread th1(printBlock, 50, '*');
    std::thread th2(printBlock, 50, '@');

    th1.join(), th2.join();
}

// Output:
// 1
// 2
// **************************************************
// or:
// 1
// **************************************************
// 2
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// or:
// 1
// **************************2
// ************************