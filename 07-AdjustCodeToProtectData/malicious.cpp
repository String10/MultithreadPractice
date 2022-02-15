#include <iostream>
#include <thread>
#include <mutex>

class Data {
    int data;
    std::string data_str;
public:
    void set_data(int k) {
        data = k;
        data_str = std::to_string(k);
    }
    void print() {
        std:: cout << data << ' ' << data_str << std::endl;
    }
};

class DataWrapper {
    Data data;
    std::mutex mtx;
public:
    template <typename Function>
    void processData(Function func) {
        std::lock_guard <std::mutex> lck(mtx);

        data.set_data(998244353);

        // 传递保护数据给用户函数
        func(data);
    }
};

Data *unprotected_data;
void maliciousFunction(Data &protected_data) {
    // 恶意拷贝数据
    unprotected_data = &protected_data;
}

int main() {
    DataWrapper wrpr;
    // 传递恶意函数
    wrpr.processData(maliciousFunction);

    unprotected_data->print();
    (*(int *)unprotected_data) = 9;
    unprotected_data->print();

}