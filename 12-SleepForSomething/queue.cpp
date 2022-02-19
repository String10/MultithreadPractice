#include <iostream>
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

template <typename T>
class ThreadsafeQueue {
    mutable std:: mutex mtx;
    std::condition_variable cv;
    std::queue <T> data;
public:
    ThreadsafeQueue() = default;
    ThreadsafeQueue(ThreadsafeQueue const& other) {
        std::lock_guard <std::mutex> lck(other.mtx);

        data = other.data;
    }
    ~ThreadsafeQueue() = default;
    void push(T new_value) {
        std::lock_guard <std::mutex> lck(mtx);
        data.push(new_value);
        
        cv.notify_one();
    }
    void waitAndPop(T &value) {
        std::unique_lock <std::mutex> lck(mtx);
        cv.wait(
            lck,
            [this]()->bool { return !data.empty(); }
        );

        value = data.front();
        data.pop();
    }
    std::shared_ptr <T> waitAndPop() {
        std::unique_lock <std::mutex> lck(mtx);
        cv.wait(
            lck,
            [this]()->bool { return !data.empty(); }
        );

        std::shared_ptr <T> res(std::make_shared <T>(data.front()));
        data.pop();
        return res;
    }
    bool tryPop(T &value) {
        std::lock_guard <std::mutex> lck(mtx);
        if(data.empty()) {
            return false;
        }
        value = data.front();
        data.pop();
        return true;
    }
    std::shared_ptr <T> tryPop() {
        std::lock_guard <std::mutex> lck(mtx);
        if(data.empty()) {
            return std::shared_ptr <T>();
        }
        std::shared_ptr <T> res(std::make_shared <T>(data.front()));
        data.pop();
        return res;
    }
    bool empty() const {
        std::lock_guard <std::mutex> lck(mtx);
        return data.empty();
    }
};

ThreadsafeQueue <int> que;

void pushNum() {
    for(int i = 0; i < 100; i++) {
        std::cout << "Push: " << i << std::endl;
        que.push(i);
    }
}
void popNum() {
    for(int i = 0; i < 100; i++) {
        int val;
        if(que.tryPop(val)) {
            std::cout << "Try pop: " << val << std::endl;
            continue;
        }
        que.waitAndPop(val);
        std::cout << "Wait pop: " << val << std::endl;
    }
}

int main() {
    std::thread th1(pushNum), th2(popNum);
    th1.join(), th2.join();
}