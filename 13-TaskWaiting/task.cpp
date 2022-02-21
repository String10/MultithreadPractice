#include <iostream>
#include <cmath>
#include <thread>
#include <future>
#include <functional>

int pwr(int x, int y) {
    return std::pow(x, y);
}

void taskLambda() {
    std::packaged_task <int(int, int)> task(pwr);
    std::future <int> res = task.get_future();

    task(2, 1);

    std::cout << "task lambda:\t" << res.get() << std::endl;
}

void taskBind() {
    std::packaged_task <int()> task(std::bind(pwr, 2, 2));
    std::future <int> res = task.get_future();
    
    task();

    std::cout << "task bind:\t" << res.get() << std::endl;
}

void taskThread() {
    std::packaged_task <int(int, int)> task(pwr);
    std::future <int> res = task.get_future();

    std::thread th_task(std::move(task), 2, 3);
    th_task.join(); // 和detach的区别？

    std::cout << "task thread:\t" << res.get() << std::endl;
}

int main() {
    taskLambda();
    taskBind();
    taskThread();
}