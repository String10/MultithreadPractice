#include <iostream>
#include <future>
#include <thread>

int main() {
    // 来自 packaged_task的future // int()应为函数类型（也许）
    std::packaged_task <int()> task(
        []() { 
            // while(true); // 如果加了这条死循环，则会一直显示Waiting...
            return 7; 
        }
    ); // 包装函数
    std::future <int> fut1 = task.get_future(); // 获取future
    std::thread(std::move(task)).detach(); // 在线程上运行

    // 来自async()的future
    // std::future <int> fut2 = std::async(std::launch::async, []() { return 8; });
    std::future <int> fut2 = std::async([]() { return 8; });

    // 来自promise的future
    std::promise <int> p;
    std::future <int> fut3 = p.get_future();
    std::thread([&p]() { p.set_value_at_thread_exit(9); }).detach();

    std::cout << "Waiting..." << std::flush;
    fut1.wait(), fut2.wait(), fut3.wait();
    std::cout << "Done!" << std::endl << "Results are: "
              << fut1.get() << fut2.get() << fut3.get() << std::endl;
}