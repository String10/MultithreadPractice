#include <iostream>
#include <thread>

class BackgroundTask {
public:
    void operator() () const {
        std::cout << "In BackgroundTask" << std::endl;
    }
};

int main() {
    // std::thread t(BackgroundTask());
    // error: parentheses were disambiguated as a function declaration
    
    // 1. use more brackets
    // std::thread t((BackgroundTask()));
    // 2. use {}
    std::thread t{BackgroundTask()};
    t.join();
    std::cout << "In main()" << std::endl;
}