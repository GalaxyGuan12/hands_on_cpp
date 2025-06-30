//
// Created by Galaxy on 2025/6/30.
//
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <windows.h>

// ==================== 1. 悬空指针的"幸运"情况 ====================

void demonstrateDanglingPointer() {
    std::cout << "=== 悬空指针演示 ===" << std::endl;

    int* ptr;
    {
        int local_var = 100;
        ptr = &local_var;
        std::cout << "作用域内，ptr指向: " << *ptr << std::endl;
        std::cout << "local_var的地址: " << &local_var << std::endl;
        std::cout << "ptr存储的地址: " << ptr << std::endl;
    } // local_var 在这里被销毁，但内存位置可能暂时未被覆盖

    std::cout << "作用域外，ptr存储的地址: " << ptr << std::endl;
    std::cout << "尝试访问悬空指针: " << *ptr << std::endl;  // 未定义行为！

    // 为什么可能还是100？因为：
    // 1. 栈内存还没有被其他变量覆盖
    // 2. 编译器可能还没有清理这块内存
    // 3. 操作系统还没有回收这个内存页
}

// ==================== 2. 展示悬空指针的危险性 ====================

void showDanglingPointerDanger() {
    std::cout << "\n=== 悬空指针的危险性 ===" << std::endl;

    int* dangerous_ptr;

    // 第一个作用域
    {
        int temp1 = 999;
        dangerous_ptr = &temp1;
        std::cout << "第一个作用域，temp1 = " << *dangerous_ptr << std::endl;
    }

    std::cout << "第一个作用域结束后: " << *dangerous_ptr << std::endl;

    // 第二个作用域 - 可能会覆盖之前的栈内存
    {
        int temp2 = 555;
        int temp3 = 777;
        std::cout << "第二个作用域，temp2 = " << temp2 << ", temp3 = " << temp3 << std::endl;
        std::cout << "temp2的地址: " << &temp2 << std::endl;
        std::cout << "temp3的地址: " << &temp3 << std::endl;
        std::cout << "dangerous_ptr的地址: " << dangerous_ptr << std::endl;
    }

    std::cout << "第二个作用域结束后，dangerous_ptr: " << *dangerous_ptr << std::endl;
    // 这时候可能会打印出555或777，因为栈内存被复用了！
}

// ==================== 3. 函数调用对栈内存的影响 ====================

void someFunction(int a, int b) {
    std::cout << "函数参数 a = " << a << ", b = " << b << std::endl;
    std::cout << "a的地址: " << &a << ", b的地址: " << &b << std::endl;

    int local = 888;
    std::cout << "函数内局部变量 local = " << local << ", 地址: " << &local << std::endl;
}

void stackMemoryReuse() {
    std::cout << "\n=== 栈内存复用演示 ===" << std::endl;

    int* ptr;
    {
        int original = 123;
        ptr = &original;
        std::cout << "原始值: " << *ptr << ", 地址: " << ptr << std::endl;
    }

    std::cout << "作用域结束后: " << *ptr << std::endl;

    // 调用函数，可能会使用相同的栈空间
    someFunction(666, 777);

    std::cout << "函数调用后，悬空指针: " << *ptr << std::endl;
    // 这时候很可能会打印出函数中的某个值！
}

// ==================== 4. 编译器优化的影响 ====================

void compilerOptimizationEffect() {
    std::cout << "\n=== 编译器优化的影响 ===" << std::endl;

    int* ptr1;
    int* ptr2;

    {
        int var1 = 111;
        int var2 = 222;
        ptr1 = &var1;
        ptr2 = &var2;

        std::cout << "作用域内:" << std::endl;
        std::cout << "var1 = " << *ptr1 << ", 地址: " << ptr1 << std::endl;
        std::cout << "var2 = " << *ptr2 << ", 地址: " << ptr2 << std::endl;
    }

    // 不同的编译器优化级别可能产生不同的结果
    std::cout << "作用域外:" << std::endl;
    std::cout << "ptr1 指向的值: " << *ptr1 << std::endl;  // 可能是111，也可能是垃圾值
    std::cout << "ptr2 指向的值: " << *ptr2 << std::endl;  // 可能是222，也可能是垃圾值
}

// ==================== 5. 堆内存 vs 栈内存对比 ====================

void heapVsStackComparison() {
    std::cout << "\n=== 堆内存 vs 栈内存对比 ===" << std::endl;

    int* stack_ptr;
    int* heap_ptr;

    // 栈内存情况
    {
        int stack_var = 100;
        stack_ptr = &stack_var;
        std::cout << "栈变量地址: " << stack_ptr << ", 值: " << *stack_ptr << std::endl;
    }

    // 堆内存情况
    {
        int* temp_heap = new int(200);
        heap_ptr = temp_heap;
        std::cout << "堆变量地址: " << heap_ptr << ", 值: " << *heap_ptr << std::endl;
        // 注意：这里没有delete，演示内存泄漏
    }

    std::cout << "作用域结束后:" << std::endl;
    std::cout << "栈指针访问: " << *stack_ptr << std::endl;  // 未定义行为
    std::cout << "堆指针访问: " << *heap_ptr << std::endl;  // 仍然有效，但造成内存泄漏

    delete heap_ptr;  // 清理堆内存
    // std::cout << *heap_ptr << std::endl;  // 现在这也是未定义行为了
}

// ==================== 6. 检测工具演示 ====================

void toolDetectionDemo() {
    std::cout << "\n=== 工具检测演示 ===" << std::endl;

    // 这种代码可以被工具检测出来
    int* ptr;
    {
        int local = 42;
        ptr = &local;
        // 静态分析工具会在这里发出警告
    }

    // 运行时工具（如AddressSanitizer）会在这里检测到错误
    std::cout << "工具检测示例，访问悬空指针: " << *ptr << std::endl;
}

// ==================== 7. 安全的替代方案 ====================

void safePractices() {
    std::cout << "\n=== 安全的替代方案 ===" << std::endl;

    // 方案1: 使用值传递而不是指针
    int value;
    {
        int local = 100;
        value = local;  // 复制值，而不是地址
    }
    std::cout << "安全的值复制: " << value << std::endl;

    // 方案2: 使用智能指针管理生命周期
    std::shared_ptr<int> smart_ptr;
    {
        auto temp = std::make_shared<int>(200);
        smart_ptr = temp;
    }
    std::cout << "智能指针管理: " << *smart_ptr << std::endl;

    // 方案3: 使用容器管理内存
    std::vector<int> container;
    {
        container.push_back(300);
    }
    std::cout << "容器管理: " << container[0] << std::endl;
}

// ==================== 8. 实际调试案例 ====================

class DebugExample {
private:
    int value_;

public:
    DebugExample(int val) : value_(val) {
        std::cout << "DebugExample(" << val << ") 构造，地址: " << this << std::endl;
    }

    ~DebugExample() {
        std::cout << "DebugExample 析构，地址: " << this << std::endl;
        value_ = -999;  // 标记为已销毁
    }

    void print() const {
        std::cout << "DebugExample::print() - value_: " << value_ << ", 地址: " << this << std::endl;
    }

    int getValue() const { return value_; }
};

void realWorldDebuggingCase() {
    std::cout << "\n=== 实际调试案例 ===" << std::endl;

    DebugExample* ptr;
    {
        DebugExample obj(42);
        ptr = &obj;
        ptr->print();
    } // obj 在这里被析构

    std::cout << "对象已析构，但指针仍然存在" << std::endl;
    std::cout << "尝试访问已析构的对象..." << std::endl;

    // 这可能会打印-999（如果内存还没被覆盖）
    // 或者程序崩溃（如果内存已被系统回收）
    ptr->print();  // 非常危险的操作！
}

int main() {
    SetConsoleOutputCP(CP_UTF8);  // 设置控制台输出为UTF-8
    std::cout << "注意：以下代码包含未定义行为，仅用于教学演示！" << std::endl;
    std::cout << "在实际项目中绝对不要写这样的代码！\n" << std::endl;

    demonstrateDanglingPointer();
    showDanglingPointerDanger();
    stackMemoryReuse();
    compilerOptimizationEffect();
    heapVsStackComparison();
    toolDetectionDemo();
    safePractices();
    realWorldDebuggingCase();

    return 0;
}