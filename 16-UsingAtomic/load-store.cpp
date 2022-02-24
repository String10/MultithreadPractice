#include <iostream>
#include <atomic>
#include <thread>

std::atomic <int> foo(0);

void set_foo(int x) {
    foo.store(x, std::memory_order_relaxed);
}

void print_foo() {
    int x;
    do {
        x = foo.load(std::memory_order_relaxed);
        std::cout << "foo: " << x << std::endl;
    } while(0 == x);
}

int main() {
    std::thread th_print(print_foo), th_set(set_foo, 9);
    th_print.join(), th_set.join();
}