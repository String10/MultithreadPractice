#include <iostream>
#include <thread>
#include <numeric>
#include <iterator>
#include <vector>
#include <algorithm>
#include <functional>

template <typename Iterator, typename T>
struct accumulateBlock {
    void operator() (Iterator first, Iterator last, T &res) {
        res = std::accumulate(first, last, res);
    }
};

template <typename Iterator, typename T>
T parallelAccumulate(Iterator first, Iterator last, T init) {
    unsigned long const length  = std::distance(first, last);
    if(0 == length) {
        return init;
    }

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    // 获取硬件支持的线程数量
    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    unsigned long const num_threads = std::min(
        hardware_threads != 0 ? hardware_threads : 2,
        max_threads
    );
    unsigned long const block_size = length / num_threads;

    std::vector <T> results(num_threads);
    std::vector <std::thread> threads(num_threads - 1);

    Iterator block_start = first;
    for(unsigned long i = 0; i < num_threads - 1; i++) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);

        threads[i] = std::thread{
            accumulateBlock <Iterator, T>(),
            block_start,
            block_end,
            std::ref(results[i])
        };

        block_start = block_end;
    }

    accumulateBlock <Iterator, T>() (
        block_start,
        last,
        results.back()
    );

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(), results.end(), init);
}

int main() {
    const int nums_size = 10000;

    std::vector <long long> nums(nums_size);
    for(unsigned long i = 0; i < nums_size; i++) {
        nums[i] = i;
    }

    std::cout << parallelAccumulate(nums.begin(), nums.end(), 0) << std::endl;
}