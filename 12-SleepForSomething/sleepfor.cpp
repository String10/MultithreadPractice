#include <iostream>
#include <thread>
#include <mutex>

bool flag = false;
std::mutex mtx;

void waitForFlag() {
    std::unique_lock <std::mutex> lck(mtx);
    while(!flag) {
        lck.unlock();

        // 休眠100ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << '@';

        lck.lock();
    }
}

void setFlag() {
    for(int i = 0; i < 10; i++) {
        std::cout << '#';
    }
    flag = true;
}

int main() {
    std::thread th1(waitForFlag), th2(setFlag);
    th1.join(), th2.join();
    std::cout << std::endl;
}