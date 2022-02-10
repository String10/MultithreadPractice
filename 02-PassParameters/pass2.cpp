#include <iostream>
#include <thread>

struct BigObject {
    long long data;
    void prepare_data(long long data_) {
        data = data_;
    }
};

void processBigObject(std::unique_ptr <BigObject> p) {
    // do something
    std::cout << p->data << std::endl;
}

int main() {
    std::unique_ptr <BigObject> p(new BigObject);
    p->prepare_data(998244353);

    // 使用move函数，进行移动语义，右值引用；将指针的所有权，交给线程内部的函数库
    std::thread t(processBigObject, std::move(p));
    t.join();

    // std::cout << p->data << std::endl; // cause a segmentation fault
}