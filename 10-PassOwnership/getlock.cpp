#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;

int num = 0;

std::unique_lock <std::mutex> getLock() {
    std::unique_lock <std::mutex> lck(mtx);

    // prepare_data
    num++;

    return lck;
}

void processData() {
    std::unique_lock <std::mutex> lck(getLock());

    // do_something
    std::cout << num << std::endl;
}

int main() {
    std::thread th(processData);

    th.join();
}