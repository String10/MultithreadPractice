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
std::mutex resource_mutex;

void foo() {
    std::unique_lock <std::mutex> lck(resource_mutex);
    if(!resource_ptr) {
        resource_ptr.reset(new Object);
    }
    lck.unlock();

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