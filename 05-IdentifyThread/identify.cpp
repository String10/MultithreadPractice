#include <iostream>
#include <thread>

std::thread::id main_thread_id = std::this_thread::get_id();

void isMainThread() {
    if(main_thread_id == std::this_thread::get_id()) {
        std::cout << "This is the main thread." << std::endl;
    }
    else {
        std::cout << "!" << std::this_thread::get_id() << std::endl;
        std::cout << "This is not the main thread." << std::endl;
    }
}

int main() {
    isMainThread();

    std::thread t(isMainThread);
    std::cout << t.get_id() << std::endl;
    t.join();

    std::cout << main_thread_id << std::endl;
}