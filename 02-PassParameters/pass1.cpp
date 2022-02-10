#include <iostream>
#include <thread>

void inc(int &num) {
    num++;
}

int main() {
    int num = 0;
    std::cout << num << std::endl;

    std::thread t(inc, std::ref(num));
    t.join();

    std::cout << num << std::endl;
}