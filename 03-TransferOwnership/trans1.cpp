#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>

struct func {
    int &i;
    func(int &i_): i(i_) {}
    void operator() () {
        for(int j = 0; j < 1000000; j++) {
            i++;
        }
    }
};

class ScopedThread {
    std::thread t;
public:
    explicit ScopedThread(std::thread t_): t(std::move(t_)) {
        if(!t.joinable()) { // 如果线程已经执行完毕就初始化失败
            throw std::logic_error("No thread");
        }
    }

    ~ScopedThread() {
        t.join();
    }

    ScopedThread(ScopedThread const&) = delete;
    ScopedThread& operator= (ScopedThread const&) = delete;
};

void f() {
    int some_local_state = 0;

    ScopedThread t((std::thread(func(some_local_state))));

    // do something in current thread
    std::cout << some_local_state << std::endl;
} // 在ScopedThread析构时运行func

void do_work(unsigned id) {
    std::cout << id << ' ';
}

void f1() {
    std::vector <std::thread> threads;
    for(unsigned i = 0; i < 20; i++) {
        threads.push_back(std::thread(do_work, i));
    }
    // std::mem_fn可以直接获取对象的函数指针
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

int main() {
    f();
    f1();
}