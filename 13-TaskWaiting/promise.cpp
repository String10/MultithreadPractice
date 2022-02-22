#include <iostream>
#include <thread>
#include <future>
#include <numeric>
#include <chrono>
#include <vector>
#include <exception>

void accumulate(std::vector <int>::iterator first,
                std::vector <int>::iterator last,
                std::promise <int> accumulate_promise) {
    int sum = std::accumulate(first, last, 0);
    accumulate_promise.set_value(sum);
}

void doWork(std::promise <void> barrier) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    barrier.set_value_at_thread_exit();
}

int division(int a, int b) {
    if(0 == b) {
        throw std::logic_error("integer division by zero");
    }
    return a / b;
}

void doCalculate(int a, int b, std::promise <int> calc_promise) {
    try {
        calc_promise.set_value(division(a, b));
    }
    catch(...) {
        calc_promise.set_exception(std::current_exception());

        std::cout << "Oops!" << std::endl;
    }
}

int main() {
    std::vector <int> nums = { 1, 2, 3, 4, 5, 6 };
    std::promise <int> accumulate_promise;
    std::future <int> accumulate_future = accumulate_promise.get_future();

    std::thread work_thread(accumulate, nums.begin(), nums.end(), std::move(accumulate_promise));

    // future::get()将等待直至该future拥有合法结果并取得它
    // 无需在get()前调用wait()
    std::cout << "results = " << accumulate_future.get() << std::endl;
    work_thread.join();

    // 演示用promise <void>在线程间对状态发信号
    std::promise <void> barrier;
    std::future <void> barrier_future = barrier.get_future();
    
    std::thread new_work_thread(doWork, std::move(barrier));
    barrier_future.wait();
    std::cout << "Wake up." << std::endl; // 在1s后输出才会这句话
    new_work_thread.join();

    std::promise <int> calc_promise;
    std::future <int> calc_future = calc_promise.get_future();
    
    std::thread calc_thread(doCalculate, 1, 0, std::move(calc_promise));
    std::cout << "Waiting..." << std::endl;
    std::cout << calc_future.get() << std::endl; // 如果不调用get()就不会产生这个异常，程序会正常退出
    calc_thread.join();
}