#include <iostream>
#include <thread>

class BackgroundTask {
public:
    void operator() () const {
        std::cout << "In BackgroundTask" << std::endl;
    }
};

int main() {
    BackgroundTask f;
    std::thread t(f);
    t.join();
    std::cout << "In main()" << std::endl;
}