#include <iostream>
#include <thread>
#include <mutex>
#include <exception>
#include <memory>
#include <stack>

/* Definition of stack
template <typename T, typename Container=std::deque <T>>
class Stack {
public:
    explicit Stack(const Container&);
    explicit Stack(Container&& = Container());
    
    template <class Alloc>
    explicit Stack(const Alloc&);
    template <class Alloc>
    Stack(const Container&, const Alloc&);
    template <class Alloc>
    Stack(Container&&, const Alloc&);
    template <class Alloc>
    Stack(Stack&&, const Alloc&);

    bool empty() const;
    size_t size() const;
    T& top();
    T const& top() const;
    void push(T const&);
    void push(T&&);
    void pop();
    void swap(Stack&&);
};
*/

struct emptyStack: std::exception {
    const char* what() const throw() {
        return "empty stack";
    }
};

template <typename T>
class threadsafeStack {
    std::stack <T> data;
    mutable std::mutex mtx;
public:
    threadsafeStack(): data(std::stack <T>()) {}
    threadsafeStack(const threadsafeStack &other) {
        std::lock_guard <std::mutex> lck(other.mtx);
        data = other.data;
    }
    threadsafeStack& operator= (const threadsafeStack&) = delete;

    void push(T value) {
        std::lock_guard <std::mutex> lck(mtx);
        data.push(value);
    }
    std::shared_ptr <T> pop() {
        std::lock_guard <std::mutex> lck(mtx);
        if(data.empty()) {
            throw emptyStack();
        }
        std::shared_ptr <T> const p_top(std::make_shared <T>(data.top()));
        data.pop();

        return p_top;
    }
    void pop(T &value) {
        std::lock_guard <std::mutex> lck(mtx);
        if(data.empty()) {
            throw emptyStack();
        }
        value = data.top();
        data.pop();
    }
    bool empty() const {
        std::lock_guard <std::mutex> lck(mtx);
        return data.empty();
    }
    T top() const {
        // std::stack::top()正常返回的是T&类型，但是为了线程安全
        // 此处返回一个data.top()的拷贝，如需要对栈顶元素进行修改，
        // 需要另外实现一个类似set_top()的线程安全的方法。
        std::lock_guard <std::mutex> lck(mtx);
        return data.top();
    }
};

// 感觉没有什么区别
// #define TEST_THREADSAFE_STACK
#define TEST_STD_STACK

#ifdef TEST_THREADSAFE_STACK
threadsafeStack <int> ts_sta;
#endif // TEST_THREADSAFE_STACK
#ifdef TEST_STD_STACK
std::stack <int> std_sta;
#endif // TEST_STD_STACK

void push_1_10() {
#ifdef TEST_THREADSAFE_STACK
    for(int i = 1; i <= 10; i++) {
        ts_sta.push(i);
    }
    for(int i = 1; i <= 10; i++) {
        int value;
        ts_sta.pop(value);
        std::cout << "In push_1_10" << ' ' << value << std::endl;
    }
#endif // TEST_THREADSAFE_STACK
#ifdef TEST_STD_STACK
    for(int i = 1; i <= 10; i++) {
        std_sta.push(i);
    }
    for(int i = 1; i <= 10; i++) {
        int value = std_sta.top();
        std_sta.pop();
        std::cout << "In push_1_10" << ' ' << value << std::endl;
    }
#endif // TEST_STD_STACK
}

void push_11_20() {
#ifdef TEST_THREADSAFE_STACK
    for(int i = 11; i <= 20; i++) {
        ts_sta.push(i);
    }
    for(int i = 1; i <= 10; i++) {
        std::cout << "In push_11_20" << ' ' << *(ts_sta.pop()) << std::endl;
    }
#endif // TEST_THREADSAFE_STACK
#ifdef TEST_STD_STACK
    for(int i = 11; i <= 20; i++) {
        std_sta.push(i);
    }
    for(int i = 1; i <= 10; i++) {
        int value = std_sta.top();
        std_sta.pop();
        std::cout << "In push_11_20" << ' ' << value << std::endl;
    }
#endif // TEST_STD_STACK
}

int main() {
    std::thread t1(push_1_10), t2(push_11_20);
    t1.join(), t2.join();
}