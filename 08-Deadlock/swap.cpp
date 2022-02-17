#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

class SomeBigObject {
    int data;
public:
    SomeBigObject(int data_): data(data_) {}
};

void swapBigObj(SomeBigObject &lhs, SomeBigObject &rhs) {
    SomeBigObject helper = lhs;
    lhs = rhs;
    rhs = helper;
}

class Object;
void swapObj(Object &lhs, Object &rhs);

class Object {
    SomeBigObject some_detail;
    std::mutex mtx;
public:
    Object(SomeBigObject const& obj): some_detail(obj) {}
    ~Object() = default;
    friend void swapObj(Object &lhs, Object &rhs) {
        if(&lhs == &rhs) {
            return;
        }

        // 1
        // std::scoped_lock lck(lhs.mtx, rhs.mtx);

        // 2
        std::lock(lhs.mtx, rhs.mtx);
        std::lock_guard <std::mutex> lck_lhs(lhs.mtx, std::adopt_lock);
        for(int i = 0; i < 100000; i++);
        std::lock_guard <std::mutex> lck_rhs(rhs.mtx, std::adopt_lock);

        // 3 error: here will cause a deadlock
        // std::lock_guard <std::mutex> lck_lhs(lhs.mtx);
        // for(int i = 0; i < 100000; i++);
        // std::lock_guard <std::mutex> lck_rhs(rhs.mtx);

        swapBigObj(lhs.some_detail, rhs.some_detail);
    }
};

Object a(SomeBigObject{9}), b(SomeBigObject{1000});

void foo0() {
    swapObj(a, b);
}
void foo1() {
    swapObj(b, a);
}

int main() {
    std::vector <std::thread> threads;
    for(int i = 0; i < 100; i++) {
        threads.push_back(std::thread(foo0));
    }
    for(int i = 0; i < 100; i++) {
        threads.push_back(std::thread(foo1));
    }
    for(auto &t: threads) {
        t.join();
    }
}