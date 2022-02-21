#include <iostream>
#include <thread>
#include <future>
#include <functional>


int getData(int &data) {
    return ++data;
}

int setData(int k, int &data) {
    data = k;
    return data;
}

int main() {
    int data = 0;

    std::future <int> fut1 = std::async(std::launch::deferred, getData, std::ref(data));
    std::future <int> fut2 = std::async(std::launch::deferred, setData, 114514, std::ref(data));

    std::cout << data << std::endl;
    std::cout << fut2.get() << std::endl;
    std::cout << fut1.get() << std::endl;

    // results:
    // fut1 use async:
    // 0
    // 114514
    // 1
    // fut1 use deferred:
    // 0
    // 114514
    // 114515
}