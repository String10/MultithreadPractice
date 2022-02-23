#include <iostream>
#include <future>
#include <thread>
#include <chrono>

int main() {
    std::future <int> fut = std::async(std::launch::async, []()->int {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 8;
    });

    std::cout << "Waiting..." << std::endl;
    std::future_status stat;
    do {
        stat = fut.wait_for(std::chrono::seconds(1));
        switch(stat) {
        case std::future_status::deferred: {
            // 共享状态持有的函数正在延迟运行，结果将仅在显式请求时计算
            std::cout << "deferred" << std::endl;
            break;
        }
        case std::future_status::timeout: {
            // 共享状态在经过指定的等待时间内仍未就绪
            std::cout << "timeout" << std::endl;
            break;
        }
        case std::future_status::ready: {
            std::cout << "ready!" << std::endl;
            break;
        }
        default: break;
        }
    } while(stat != std::future_status::ready);

    std::cout << "result is " << fut.get() << std::endl;
}