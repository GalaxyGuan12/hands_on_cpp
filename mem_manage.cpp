//
// Created by Galaxy on 2025/6/30.
//
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <windows.h>

class TestObject {
private:
    int data[100];  // 占用400字节

public:
    TestObject(int value = 0) {
        for (int i = 0; i < 100; i++) {
            data[i] = value + i;
        }
    }

    int getSum() const {
        int sum = 0;
        for (int i = 0; i < 100; i++) {
            sum += data[i];
        }
        return sum;
    }
};

// 栈分配示例
void stackAllocationDemo() {
    std::cout << "=== 栈分配示例 ===" << std::endl;

    // 在栈上分配基本类型
    int stackInt = 42;
    double stackDouble = 3.14;
    char stackArray[1000];  // 在栈上分配数组

    // 在栈上分配对象
    TestObject stackObj(10);

    std::cout << "栈变量地址:" << std::endl;
    std::cout << "stackInt地址: " << &stackInt << std::endl;
    std::cout << "stackDouble地址: " << &stackDouble << std::endl;
    std::cout << "stackArray地址: " << stackArray << std::endl;
    std::cout << "stackObj地址: " << &stackObj << std::endl;

    std::cout << "stackObj.getSum() = " << stackObj.getSum() << std::endl;

    // 栈变量在函数结束时自动销毁
}

// 堆分配示例
void heapAllocationDemo() {
    std::cout << "\n=== 堆分配示例 ===" << std::endl;

    // 在堆上分配基本类型
    int* heapInt = new int(42);
    double* heapDouble = new double(3.14);
    char* heapArray = new char[1000];

    // 在堆上分配对象
    TestObject* heapObj = new TestObject(20);

    std::cout << "堆变量地址:" << std::endl;
    std::cout << "heapInt地址: " << heapInt << ", 值: " << *heapInt << std::endl;
    std::cout << "heapDouble地址: " << heapDouble << ", 值: " << *heapDouble << std::endl;
    std::cout << "heapArray地址: " << heapArray << std::endl;
    std::cout << "heapObj地址: " << heapObj << std::endl;

    std::cout << "heapObj->getSum() = " << heapObj->getSum() << std::endl;

    // 手动释放堆内存
    delete heapInt;
    delete heapDouble;
    delete[] heapArray;
    delete heapObj;
}

// 性能对比
void performanceComparison() {
    std::cout << "\n=== 性能对比 ===" << std::endl;

    const int iterations = 1000000;

    // 栈分配性能测试
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        TestObject stackObj(i);
        volatile int sum = stackObj.getSum();  // 防止编译器优化
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto stackTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // 堆分配性能测试
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        TestObject* heapObj = new TestObject(i);
        volatile int sum = heapObj->getSum();
        delete heapObj;
    }
    end = std::chrono::high_resolution_clock::now();
    auto heapTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "栈分配时间: " << stackTime.count() << " 微秒" << std::endl;
    std::cout << "堆分配时间: " << heapTime.count() << " 微秒" << std::endl;
    std::cout << "堆分配比栈分配慢: " << (double)heapTime.count() / stackTime.count() << " 倍" << std::endl;
}

// 何时使用栈vs堆的示例
void usageGuidelines() {
    std::cout << "\n=== 使用指导 ===" << std::endl;

    // ✅ 适合栈分配的场景
    {
        std::cout << "栈分配适用场景:" << std::endl;

        // 1. 小型局部变量
        int counter = 0;
        double temperature = 36.5;

        // 2. 小型数组（大小确定）
        char buffer[256];
        int numbers[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // 3. 临时对象
        TestObject tempObj(42);

        // 4. 函数参数和返回值
        auto processData = [](int value) -> TestObject {
            return TestObject(value);  // 返回栈对象
        };

        TestObject result = processData(100);
        std::cout << "- 小型变量和临时对象" << std::endl;
        std::cout << "- 大小确定的小型数组" << std::endl;
        std::cout << "- 函数参数和返回值" << std::endl;
    }

    // ✅ 适合堆分配的场景
    {
        std::cout << "\n堆分配适用场景:" << std::endl;

        // 1. 大型数据结构
        std::vector<TestObject> largeVector;
        largeVector.reserve(10000);  // 大型容器

        // 2. 动态大小的数据
        int size = 1000;  // 运行时确定大小
        std::unique_ptr<int[]> dynamicArray = std::make_unique<int[]>(size);

        // 3. 需要在函数间传递的对象
        std::shared_ptr<TestObject> sharedObj = std::make_shared<TestObject>(200);

        // 4. 生命周期不确定的对象
        std::unique_ptr<TestObject> longLivedObj = std::make_unique<TestObject>(300);

        std::cout << "- 大型数据结构" << std::endl;
        std::cout << "- 动态大小的数据" << std::endl;
        std::cout << "- 需要在函数间传递的对象" << std::endl;
        std::cout << "- 生命周期不确定的对象" << std::endl;
    }
}

// 栈溢出示例（危险！实际使用时要小心）
void stackOverflowDemo() {
    std::cout << "\n=== 栈溢出示例（仅演示，实际使用需谨慎） ===" << std::endl;

    try {
        // 尝试在栈上分配过大的数组
        // 注意：这可能导致程序崩溃！
        const int LARGE_SIZE = 1000000;  // 4MB左右
        // int largeArray[LARGE_SIZE];  // 取消注释可能导致栈溢出

        std::cout << "如果取消注释上面的代码，可能会导致栈溢出" << std::endl;
        std::cout << "建议使用堆分配或vector来处理大型数据" << std::endl;

        // 正确的做法：使用堆分配
        std::vector<int> largeVector(LARGE_SIZE);
        std::cout << "使用vector成功分配了 " << LARGE_SIZE << " 个整数" << std::endl;

    } catch (...) {
        std::cout << "捕获到异常" << std::endl;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);  // 设置控制台输出为UTF-8
    stackAllocationDemo();
    heapAllocationDemo();
    performanceComparison();
    usageGuidelines();
    stackOverflowDemo();

    std::cout << "\n总结:" << std::endl;
    std::cout << "- 栈：快速、自动管理、容量有限" << std::endl;
    std::cout << "- 堆：灵活、手动管理、容量大" << std::endl;
    std::cout << "- 优先使用栈，需要时使用堆" << std::endl;
    std::cout << "- 使用智能指针管理堆内存" << std::endl;

    return 0;
}