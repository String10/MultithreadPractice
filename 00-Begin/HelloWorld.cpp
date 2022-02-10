#include <iostream>
#include <thread>

void sayHello() {
    std::cout << " world!" << std::endl;
}

int main() {
    std:: thread t(sayHello);
    std::cout << "Hello";
    t.join();
}