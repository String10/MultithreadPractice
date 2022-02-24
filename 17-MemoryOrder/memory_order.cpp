#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>

// Acquire-Release / relaxed 语义
class CAcqRel {
    int m;
    std::atomic <bool> x, y;
    std::atomic <int> z;
public:
    CAcqRel(): x(false), y(false), z(0), m(0) {}
    void write_x_then_y() {
        m = 1; // 非原子变量

        // relaxed 语义，无法保证m和x写入的先后顺序
        x.store(true, std::memory_order_relaxed);
        // 对x实施release语义，保证m/x一定在y之前被写入
        y.store(true, std::memory_order_release);
    }
    void read_y_then_x() {
        // 对y实施acquire语义，同时使用循环
        // 等待y原子变量的release的发生
        while(!y.load(std::memory_order_acquire));
        // acquire线程可见到release之前的写操作，因此x为true
        if(x.load(std::memory_order_relaxed)) {
            ++z;
        }
    }
    void test() {
        std::thread a(&CAcqRel::write_x_then_y, this);
        std::thread b(&CAcqRel::read_y_then_x, this);

        a.join(), b.join();
        assert(0 != z.load());
        assert(1 == m);
    }
};

// Consume语义
// 类似memory_order_acquire
// 但只对与这块内存有关的读写操作起作用
class CConsume {
    std::atomic <std::string *> ptr;
    int data;
    std::atomic <int> at_data;
public:
    CConsume(): data(0), at_data(0), ptr(nullptr) {}
    void producer() {
        std::string *p = new std::string("Hello");
        data = 42;
        at_data.store(2022, std::memory_order_relaxed);

        ptr.store(p, std::memory_order_release);
    }
    void consumer() {
        std::string *p;
        // consume语义，只能保证ptr依赖的变量p已被存储
        // 但不保证data和at_data的值
        while(nullptr == (p = ptr.load(std::memory_order_consume)));
        
        assert("Hello" == (*p)); // 一定成立
        assert(42 == data); // 无法保证data一定等于42，因为ptr对其无依赖
        assert(2022 == at_data); // 无法保证data一定等于42，因为ptr对其无依赖
    }
    void test() {
        std::thread t1(&CConsume::producer, this);
        std::thread t2(&CConsume::consumer, this);
        t1.join(), t2.join();
    }
};

// Seq-Cst语义
// 如果是读取就是acquire语义
// 写入就是release语义
// 读写就施加acquire-release语义。
class CSeqCst {
    std::string work;
    std::atomic <bool> ready;
    std::atomic <int> data;
public:
    CSeqCst(): ready(false), data(0) {}
    void write() {
        // 以下的写操作采用memory_order_seq_cst语义
        // 因此当写入时会产生一个全局的写入顺序，即先work再data，最后写入ready
        // 这个顺序对所有使用该语义的线程可见
        work = "done";
        data.store(2015, std::memory_order_seq_cst);
        ready = true; // default = memory_order_seq_cst
    }
    void read() {
        // 默认为memory_order_seq_cst语义
        // 当ready发生时
        // 由于全局顺序必然知道data和work的存储已经发生
        while(!(ready.load()));

        std::cout << work << std::endl;
        std::cout << data << std::endl;
    }
    void test() {
        std::thread t1(&CSeqCst::write, this);
        std::thread t2(&CSeqCst::read, this);

        t1.join(), t2.join();
    }
};

// Memory Barriers
class CMemoryBarriers {
    bool x = false;
    std::atomic <bool> y;
    std::atomic <int> z;
public:
    CMemoryBarriers(): x(false), y(false), z(0) {}
    void write_x_then_y() {
        // 1) 在栅栏前存储x
        x = true; 
        // 2) 释放栅栏
        std::atomic_thread_fence(std::memory_order_release); 
        // 3) 在栅栏之后存储y
        y.store(true, std::memory_order_relaxed); 
    }
    void read_y_then_x() {
        // 4) 在(3)写入前持续等待
        while(!y.load(std::memory_order_relaxed)); 
        // 5) 读取栅栏
        // (2)和(5)是同步关系，且都是必要的，这样才能在两个线程间建立同步关系
        std::atomic_thread_fence(std::memory_order_acquire);
        // 6) 这里的x读取是在(5)后，由于栅栏之间的同步关系
        // 也发生在(1)之后
        // 所以这里读取到的值是(1)写入的
        if(x) {
            ++z;
        }
    }
    void test() {
        std::thread a(&CMemoryBarriers::write_x_then_y, this);
        std::thread b(&CMemoryBarriers::read_y_then_x, this);

        a.join(), b.join();
        assert(0 != z.load());
    }
};

int main() {
    CAcqRel ar;
    ar.test();

    CConsume cs;
    cs.test();

    CSeqCst sc;
    sc.test();

    CMemoryBarriers mb;
    mb.test();
}