#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::mutex mtx;
std::condition_variable data_cond; // 环境监测变量
std::queue <int> data_queue; // 数据队列

int prepareData() {
    static int data_ = 0;
    return data_++;
}

bool moreDataToPrepare() {
    return data_queue.size() < 10000;
}

void dataPrepareThread() {
    std::unique_lock <std::mutex> lck(mtx);

    while(moreDataToPrepare()) {
        int const data = prepareData();
        std::cout << "Preparing data..." << std::endl;
        data_queue.push(data);

    }

    // Unblocks one of the threads currently waiting for this condition.
    data_cond.notify_one();
}

void dataProcessingThread() {
    while(true) {
        std::unique_lock <std::mutex> lck(mtx);

        // which means:
        // while (!pred()) wait(lck);
        data_cond.wait(
            lck,
            []()->bool { return !data_queue.empty(); } // called repeatedly until it evaluates to true.
        );

        std::cout << "Processing data..." << std::endl;

        int data = data_queue.front();
        data_queue.pop();
        std::cout << data_queue.size() << ' ' << data << std::endl;

        lck.unlock();
    }
}

int main() {
    // 准备好数据之前一直等待
    std::thread th1(dataPrepareThread), th2(dataProcessingThread);
    th1.join(), th2.join();
}