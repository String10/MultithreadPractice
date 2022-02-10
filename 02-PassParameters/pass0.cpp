#include <iostream>
#include <string>
#include <thread>
#include <cstdio>

void f(int num, std::string const &s) {
    std::cout << num << ' ' << s << std::endl;
}

void oops(int num) {
    char buffer[1024];
    sprintf(buffer, "%d", num);

    std::thread t(f, num, buffer);
    t.detach();
} // std::thread的构造函数会复制提供的变量，但是函数有可能在类型转换未完成时结束，从而导致崩溃

void good(int num) {
    char buffer[1024];
    sprintf(buffer, "%d", num);

    // 使用显示转换避免指针悬垂
    std::thread t(f, num, std::string(buffer));
    t.detach();
}

int main() {
    // oops(998244353);
    good(998244353);
}