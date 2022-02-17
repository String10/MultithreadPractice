#include <iostream>
#include <thread>
#include <mutex>
#include <exception>

class HierarchicalMutex {
    std::mutex internal_mutex;
    unsigned long const hierarchical_value; // 接受的层级值
    unsigned long previous_hierarchy_value; // 原来的层级值
    static thread_local unsigned long this_thread_hierarchy_value; // 当前线程的层级值
    void checkForHierarchyViolation() {
        // 检查层级值以保持递减的序列
        if(this_thread_hierarchy_value <= hierarchical_value) {
            throw std::logic_error("mutex hierarchy violated");
        }
    }
    void updateHierachyValue() {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchical_value;
    }
public:
    explicit HierarchicalMutex(unsigned long value): 
        hierarchical_value(value), previous_hierarchy_value(0) {};
    ~HierarchicalMutex() = default;
    void lock() {
        // 确认当前值小于新值
        checkForHierarchyViolation();
        
        // 当前线程加锁
        internal_mutex.lock();
        updateHierachyValue();
    }
    void unlock() {
        // 还原原来的层级值
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }
    bool try_lock() {
        checkForHierarchyViolation();
        if(!internal_mutex.try_lock()) {
            return false;
        }
        updateHierachyValue();
        return true;
    }
};
// 直接将信号变量初始化为最大值
thread_local unsigned long HierarchicalMutex::this_thread_hierarchy_value(0x3f3f3f3f);

// 使用层级锁来避免死锁

HierarchicalMutex high_level_mutex(10000); // 层级锁，层级为10000（较大），需要优先被上锁
HierarchicalMutex low_level_mutex(5000); // 层级锁，层级为5000（较小），需要延后被上锁

int doLowLevelStuff() {
    return 10;
}
int lowLevelFunc() {
    std::lock_guard <HierarchicalMutex> lck(low_level_mutex);
    return doLowLevelStuff();
}

void highLevelStuff(int some_param) {
    some_param++;
}
void highLevelFunc() {
    std::lock_guard <HierarchicalMutex> lck(high_level_mutex);
    highLevelStuff(lowLevelFunc());
}

void thread_a() { // 线程a遵守层级规则，所以运行没有问题
    highLevelFunc();
}

HierarchicalMutex other_mutex(100);
void doOtherStuff() {
    return;
}
void otherFunc() {
    highLevelFunc();
    doOtherStuff();
}

void thread_b() { // 线程b无视层级规则，other_mutex被上锁意味着底层级已经被保护，不能再对层级上锁
    std::lock_guard <HierarchicalMutex> lck(other_mutex);
    otherFunc();
}

int main() {
    std::thread t(thread_b);
    t.join();
}