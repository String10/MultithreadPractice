#include <iostream>
#include <thread>

struct func {
    int &i;
    func(int &i_): i(i_) {}
    void operator() () {
        for(int j = 0; j < 1000000; j++) {
            i++;
            // 1. 潜在访问隐患：悬空引用
        }
    }
};

void oops() {
    int local_i = 0;
    func f(local_i);
    std::thread t{f};
    t.detach(); // 2. 不等待线程结束
} // 3. 新线程可能还在运行（此时local_i已经销毁）

void sol0() {
    int local_i = 0;
    func f(local_i);
    std::thread t{f};

    // 使用异常捕获，保证访问本地状态的线程退出后，函数才结束
    try {
        std::cout << local_i << std::endl;
    } catch(...) {
        t.join();
        throw;
    }
    t.join();
}

// 使用“资源获取即初始化方式”(RAII,Resource Acquisition Is Initialization)等待线程退出
class ThreadGuard {
public:
    explicit ThreadGuard(std::thread &t_): t(t_) {}
    ~ThreadGuard() {
        if(t.joinable()) {
            t.join();
        }
    }
    ThreadGuard(ThreadGuard&) = delete;
    ThreadGuard& operator= (ThreadGuard const&) = delete;
private:
    std::thread &t;
};

void sol1() {
    int local_i = 0;
    func f(local_i);
    std::thread t{f};
    ThreadGuard g(t);
    std::cout << local_i << std::endl;
} // 函数结束，局部对象开始逆序销毁

int main() {
    sol1();
}