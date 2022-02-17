#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <vector>

class Object {
    int data;
    static int counter;
public:
    Object(): data(0) {
        counter++;
    }
    void doSomething() {
        std::cout << counter << ' ' << data << std::endl;
    }
};

int Object::counter = 0;

std::shared_ptr <Object> resource_ptr;
std::once_flag resource_flag;

void initResource() {
    resource_ptr.reset(new Object);
}

void foo() {
    // 使用std::call_once比显式使用互斥量消耗的资源更少,特别是当初始化完成后
    std::call_once(resource_flag, initResource);

    resource_ptr->doSomething();
}

int main() {
    std::vector <std::thread> threads;
    for(int i = 0; i < 5; i++) {
        threads.push_back(std::thread(foo));
    }
    for(auto &th: threads) {
        th.join();
    }
}