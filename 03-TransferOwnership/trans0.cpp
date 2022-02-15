#include <iostream>
#include <thread>

void some_function() {
    for(int i = 0; i < 10000; i++) {
        std::cout << i << ' ';
    }
}

void some_other_function() {
    for(int i = 0; i < 10000; i++) {
        std::cout << -i << ' ';
    }
}

int main() {
    // 直接构造线程对象
    std::thread t1(some_function);

    // t1.join(); // 如果赋值时t1正在运行则赋值失败

    // 移动线程句柄到t2，之后t1无用
    std::thread t2 = std::move(t1);

    t1 = std::thread(some_other_function);

    std::thread t3;

    t3 = td::move(t2);

    // t1执行std::terminate()终止线程继续运行，保证与std::thread的析构函数行为一致
    t1 = std::move(t3);
}
