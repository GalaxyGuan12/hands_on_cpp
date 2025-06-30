//
// Created by sim on 2025/6/30.
//

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <windows.h>

// ==================== 1. 指针和引用的基本区别 ====================

void pointerVsReference() {
    std::cout << "=== 指针 vs 引用 ===" << std::endl;

    int a = 10, b = 20;

    // 指针示例
    int* ptr = &a;          // 指针可以为空，可以重新赋值
    std::cout << "指针指向a: " << *ptr << std::endl;
    ptr = &b;               // 指针可以改变指向
    std::cout << "指针指向b: " << *ptr << std::endl;
    ptr = nullptr;          // 指针可以为空

    // 引用示例
    int& ref = a;           // 引用必须初始化，不能为空
    std::cout << "引用a: " << ref << std::endl;
    // int& ref2;           // 错误：引用必须初始化
    // ref = b;             // 这是赋值操作，不是改变引用指向
    ref = 30;               // 实际上是给a赋值
    std::cout << "修改引用后a的值: " << a << std::endl;
}

// ==================== 2. 智能指针示例 ====================

class Resource {
public:
    Resource(int id) : id_(id) {
        std::cout << "Resource " << id_ << " 构造" << std::endl;
    }

    ~Resource() {
        std::cout << "Resource " << id_ << " 析构" << std::endl;
    }

    void use() const {
        std::cout << "使用 Resource " << id_ << std::endl;
    }

private:
    int id_; // 私有成员变量建议增加下划线后缀，便于区分
};

void smartPointerExamples() {
    std::cout << "\n=== 智能指针示例 ===" << std::endl;

    // unique_ptr - 独占所有权
    {
        std::cout << "--- unique_ptr ---" << std::endl;
        std::unique_ptr<Resource> uptr = std::make_unique<Resource>(1);
        uptr->use();

        // std::unique_ptr<Resource> uptr2 = uptr;  // 错误：不能拷贝
        std::unique_ptr<Resource> uptr2 = std::move(uptr);  // 移动语义

        if (!uptr) {
            std::cout << "uptr 已经被移动，现在为空" << std::endl;
        }
        uptr2->use();
    } // uptr2 在此处自动析构

    // shared_ptr - 共享所有权
    {
        std::cout << "\n--- shared_ptr ---" << std::endl;
        std::shared_ptr<Resource> sptr1 = std::make_shared<Resource>(2);
        std::cout << "引用计数: " << sptr1.use_count() << std::endl;

        {
            std::shared_ptr<Resource> sptr2 = sptr1;  // 可以拷贝
            std::cout << "引用计数: " << sptr1.use_count() << std::endl;
            sptr2->use();
        } // sptr2 离开作用域，引用计数减1

        std::cout << "引用计数: " << sptr1.use_count() << std::endl;
    } // sptr1 离开作用域，对象被析构

    // weak_ptr - 解决循环引用
    {
        std::cout << "\n--- weak_ptr ---" << std::endl;
        std::shared_ptr<Resource> sptr = std::make_shared<Resource>(3);
        std::weak_ptr<Resource> wptr = sptr;

        std::cout << "shared_ptr 引用计数: " << sptr.use_count() << std::endl;
        std::cout << "weak_ptr 过期了吗? " << (wptr.expired() ? "是" : "否") << std::endl;

        if (auto locked = wptr.lock()) {  // 安全访问
            locked->use();
        }

        sptr.reset();  // 释放shared_ptr
        std::cout << "shared_ptr 重置后，weak_ptr 过期了吗? " << (wptr.expired() ? "是" : "否") << std::endl;
    }
}

// ==================== 9. weak_ptr 的具体使用场景 ====================

// 场景1: 解决父子循环引用问题
class Parent;
class Child;

class Parent {
public:
    Parent(const std::string& name) : name_(name) {
        std::cout << "Parent " << name_ << " 创建" << std::endl;
    }

    ~Parent() {
        std::cout << "Parent " << name_ << " 销毁" << std::endl;
    }

    void addChild(std::shared_ptr<Child> child) {
        children_.push_back(child);
    }

    void showChildren() const;

private:
    std::string name_;
    std::vector<std::shared_ptr<Child>> children_;
};

class Child {
public:
    Child(const std::string& name) : name_(name) {
        std::cout << "Child " << name_ << " 创建" << std::endl;
    }

    ~Child() {
        std::cout << "Child " << name_ << " 销毁" << std::endl;
    }

    // 使用 weak_ptr 避免循环引用
    void setParent(std::shared_ptr<Parent> parent) {
        parent_ = parent;  // weak_ptr 不会增加引用计数
    }

    void visitParent() const {
        if (auto parent = parent_.lock()) {  // 安全访问
            std::cout << "Child " << name_ << " 访问 Parent" << std::endl;
        } else {
            std::cout << "Child " << name_ << " 的 Parent 已不存在" << std::endl;
        }
    }

    const std::string& getName() const { return name_; }

private:
    std::string name_;
    std::weak_ptr<Parent> parent_;  // 使用 weak_ptr 避免循环引用
};

void Parent::showChildren() const {
    std::cout << "Parent " << name_ << " 的子节点: ";
    for (const auto& child : children_) {
        std::cout << child->getName() << " ";
    }
    std::cout << std::endl;
}

void parentChildCircularReference() {
    std::cout << "\n=== 父子循环引用场景 ===" << std::endl;

    {
        auto parent = std::make_shared<Parent>("爸爸");
        auto child1 = std::make_shared<Child>("孩子1");
        auto child2 = std::make_shared<Child>("孩子2");

        // 建立父子关系
        parent->addChild(child1);
        parent->addChild(child2);
        child1->setParent(parent);
        child2->setParent(parent);

        parent->showChildren();
        child1->visitParent();

        std::cout << "Parent 引用计数: " << parent.use_count() << std::endl;
        // 如果 Child 使用 shared_ptr<Parent>，这里引用计数会是 3
        // 使用 weak_ptr 后，引用计数是 1
    } // 所有对象在这里正常销毁

    std::cout << "作用域结束，对象已销毁" << std::endl;
}

// 场景2: 观察者模式中的应用
class Subject;

class Observer {
public:
    Observer(int id) : id_(id) {
        std::cout << "Observer " << id_ << " 创建" << std::endl;
    }

    ~Observer() {
        std::cout << "Observer " << id_ << " 销毁" << std::endl;
    }

    void update(const std::string& message) {
        std::cout << "Observer " << id_ << " 收到消息: " << message << std::endl;
    }

    int getId() const { return id_; }

private:
    int id_;
};

class Subject {
public:
    Subject() {
        std::cout << "Subject 创建" << std::endl;
    }

    ~Subject() {
        std::cout << "Subject 销毁" << std::endl;
    }

    void attach(std::shared_ptr<Observer> observer) {
        observers_.push_back(observer);  // 使用 weak_ptr 存储观察者
    }

    void notify(const std::string& message) {
        std::cout << "Subject 通知所有观察者..." << std::endl;

        // 遍历时需要检查 weak_ptr 是否有效
        auto it = observers_.begin();
        while (it != observers_.end()) {
            if (auto observer = it->lock()) {
                observer->update(message);
                ++it;
            } else {
                // 观察者已被销毁，从列表中移除
                std::cout << "移除已销毁的观察者" << std::endl;
                it = observers_.erase(it);
            }
        }
    }

    void showObserverCount() {
        size_t active_count = 0;
        for (auto& weak_obs : observers_) {
            if (!weak_obs.expired()) {
                active_count++;
            }
        }
        std::cout << "活跃观察者数量: " << active_count << std::endl;
    }

private:
    std::vector<std::weak_ptr<Observer>> observers_;  // 使用 weak_ptr 避免强引用
};

void observerPatternExample() {
    std::cout << "\n=== 观察者模式场景 ===" << std::endl;

    auto subject = std::make_shared<Subject>();

    {
        auto obs1 = std::make_shared<Observer>(1);
        auto obs2 = std::make_shared<Observer>(2);
        auto obs3 = std::make_shared<Observer>(3);

        subject->attach(obs1);
        subject->attach(obs2);
        subject->attach(obs3);

        subject->showObserverCount();
        subject->notify("第一条消息");

        // obs2 离开作用域被销毁
        obs2.reset();
        std::cout << "\nObserver 2 被销毁后:" << std::endl;
        subject->notify("第二条消息");
    } // obs1 和 obs3 在这里被销毁

    std::cout << "\n所有观察者离开作用域后:" << std::endl;
    subject->notify("第三条消息");
}

// 场景3: 缓存系统中的应用
class CacheEntry {
public:
    CacheEntry(int id, const std::string& data) : id_(id), data_(data) {
        std::cout << "CacheEntry " << id_ << " 创建" << std::endl;
    }

    ~CacheEntry() {
        std::cout << "CacheEntry " << id_ << " 销毁" << std::endl;
    }

    const std::string& getData() const { return data_; }
    int getId() const { return id_; }

private:
    int id_;
    std::string data_;
};

class Cache {
public:
    std::shared_ptr<CacheEntry> get(int id) {
        auto it = cache_.find(id);
        if (it != cache_.end()) {
            // 尝试从 weak_ptr 获取 shared_ptr
            if (auto entry = it->second.lock()) {
                std::cout << "缓存命中: " << id << std::endl;
                return entry;
            } else {
                // 缓存项已过期，移除
                std::cout << "缓存项 " << id << " 已过期，移除" << std::endl;
                cache_.erase(it);
            }
        }

        // 创建新的缓存项
        std::cout << "创建新缓存项: " << id << std::endl;
        auto entry = std::make_shared<CacheEntry>(id, "数据" + std::to_string(id));
        cache_[id] = entry;  // 存储 weak_ptr
        return entry;
    }

    void cleanup() {
        std::cout << "清理过期缓存..." << std::endl;
        auto it = cache_.begin();
        while (it != cache_.end()) {
            if (it->second.expired()) {
                std::cout << "移除过期缓存项: " << it->first << std::endl;
                it = cache_.erase(it);
            } else {
                ++it;
            }
        }
    }

    void showCacheStatus() {
        std::cout << "缓存状态 - 总项数: " << cache_.size() << ", 活跃: ";
        int active = 0;
        for (const auto& pair : cache_) {
            if (!pair.second.expired()) active++; // 离开作用域后，std::weak_ptr的expired()方法会返回true
        }
        std::cout << active << std::endl;
    }

private:
    std::unordered_map<int, std::weak_ptr<CacheEntry>> cache_;
};

void cacheExample() {
    std::cout << "\n=== 缓存系统场景 ===" << std::endl;

    Cache cache;

    {
        auto entry1 = cache.get(1);
        auto entry2 = cache.get(2);
        cache.showCacheStatus();

        // 再次获取相同的项，应该命中缓存
        auto entry1_again = cache.get(1);

        std::cout << "entry1 和 entry1_again 是同一个对象吗? "
                  << (entry1.get() == entry1_again.get() ? "是" : "否") << std::endl;
    } // entry1, entry2, entry1_again 离开作用域

    std::cout << "\n引用离开作用域后:" << std::endl;
    cache.showCacheStatus();

    // 尝试再次获取，应该创建新对象
    auto entry3 = cache.get(1);
    cache.cleanup();  // 手动清理
    cache.showCacheStatus();
}

// ==================== 3. 野指针和悬空指针 ====================

void danglingPointerExample() {
    std::cout << "\n=== 野指针和悬空指针 ===" << std::endl;

    int* ptr;
    // std::cout << *ptr << std::endl;  // 危险：未初始化的指针（野指针）

    ptr = new int(42);
    std::cout << "分配内存后: " << *ptr << std::endl;

    delete ptr;
    // std::cout << *ptr << std::endl;  // 危险：悬空指针，指向已释放的内存

    ptr = nullptr;  // 好习惯：避免悬空指针

    // 另一种悬空指针情况
    std::cout << "=== 悬空指针演示 ===" << std::endl;

    int* ptr2;
    {
        int local_var = 100;
        ptr2 = &local_var;
        std::cout << "作用域内，ptr指向: " << *ptr2 << std::endl;
        std::cout << "local_var的地址: " << &local_var << std::endl;
        std::cout << "ptr存储的地址: " << ptr2 << std::endl;
    } // local_var 在这里被销毁，但内存位置可能暂时未被覆盖

    std::cout << "作用域外，ptr存储的地址: " << ptr2 << std::endl;
    std::cout << "尝试访问悬空指针: " << *ptr2 << std::endl;  // 未定义行为！

    // 为什么可能还是100？因为：
    // 1. 栈内存还没有被其他变量覆盖
    // 2. 编译器可能还没有清理这块内存
    // 3. 操作系统还没有回收这个内存页
}

// ==================== 4. 指针数组 vs 数组指针 ====================

void pointerArrayVsArrayPointer() {
    std::cout << "\n=== 指针数组 vs 数组指针 ===" << std::endl;

    // 指针数组：存储指针的数组
    int a = 1, b = 2, c = 3;
    int* ptr_array[3] = {&a, &b, &c};  // 3个指针的数组
    std::cout << "指针数组: ";
    for (int i = 0; i < 3; ++i) {
        std::cout << *ptr_array[i] << " ";
    }
    std::cout << std::endl;

    // 数组指针：指向数组的指针
    int arr[3] = {4, 5, 6};
    int (*array_ptr)[3] = &arr;  // 指向含有3个int的数组的指针
    std::cout << "数组指针: ";
    for (int i = 0; i < 3; ++i) {
        std::cout << (*array_ptr)[i] << " ";
    }
    std::cout << std::endl;
}

// ==================== 5. 函数指针和引用 ====================

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

void functionPointerExample() {
    std::cout << "\n=== 函数指针 ===" << std::endl;

    // 函数指针
    int (*func_ptr)(int, int) = add;
    std::cout << "函数指针调用 add(3, 4): " << func_ptr(3, 4) << std::endl;

    func_ptr = multiply;
    std::cout << "函数指针调用 multiply(3, 4): " << func_ptr(3, 4) << std::endl;

    // 函数指针数组
    int (*operations[])(int, int) = {add, multiply};
    std::cout << "函数指针数组调用: " << operations[0](5, 6) << ", " << operations[1](5, 6) << std::endl;
}

// ==================== 6. 引用作为函数参数和返回值 ====================

void swapByPointer(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void swapByReference(int& a, int& b) {
    int temp = a;   // temp 存储 a 的值的副本
    a = b;          // a 获得 b 的值
    b = temp;       // b 获得原来 a 的值（通过 temp）
}

// 返回引用的函数
int& getElement(std::vector<int>& vec, size_t index) {
    return vec[index];  // 返回引用，可以用于赋值
}

void referenceParameterExample() {
    std::cout << "\n=== 引用作为参数和返回值 ===" << std::endl;

    int x = 10, y = 20;
    std::cout << "交换前: x=" << x << ", y=" << y << std::endl;

    // 使用指针交换
    swapByPointer(&x, &y);
    std::cout << "指针交换后: x=" << x << ", y=" << y << std::endl;

    // 使用引用交换
    swapByReference(x, y);
    std::cout << "引用交换后: x=" << x << ", y=" << y << std::endl;

    // 返回引用的使用
    std::vector<int> vec = {1, 2, 3, 4, 5};
    getElement(vec, 2) = 100;  // 直接修改vector中的元素
    std::cout << "修改后的vector: ";
    for (int val : vec) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

// ==================== 7. 常量指针和指针常量 ====================

void constPointerExample() {
    std::cout << "\n=== 常量指针和指针常量 ===" << std::endl;

    int a = 10, b = 20;

    // 指向常量的指针（指针可变，指向的值不可变）
    const int* ptr1 = &a;
    std::cout << "指向常量的指针: " << *ptr1 << std::endl;
    // *ptr1 = 30;  // 错误：不能修改指向的值
    ptr1 = &b;      // 正确：指针本身可以改变
    std::cout << "改变指向后: " << *ptr1 << std::endl;

    // 常量指针（指针不可变，指向的值可变）
    int* const ptr2 = &a;
    std::cout << "常量指针: " << *ptr2 << std::endl;
    *ptr2 = 30;     // 正确：可以修改指向的值
    // ptr2 = &b;   // 错误：指针本身不能改变
    std::cout << "修改值后: " << *ptr2 << std::endl;

    // 指向常量的常量指针（指针和值都不可变）
    const int* const ptr3 = &a;
    std::cout << "指向常量的常量指针: " << *ptr3 << std::endl;
    // *ptr3 = 40;  // 错误：不能修改值
    // ptr3 = &b;   // 错误：不能修改指针
}

// ==================== 8. 多级指针 ====================

void multiLevelPointerExample() {
    std::cout << "\n=== 多级指针 ===" << std::endl;

    int value = 42;
    int* ptr = &value;
    int** ptr_to_ptr = &ptr;
    int*** ptr_to_ptr_to_ptr = &ptr_to_ptr;

    std::cout << "原始值: " << value << std::endl;
    std::cout << "通过指针访问: " << *ptr << std::endl;
    std::cout << "通过二级指针访问: " << **ptr_to_ptr << std::endl;
    std::cout << "通过三级指针访问: " << ***ptr_to_ptr_to_ptr << std::endl;

    // 动态分配二维数组
    int rows = 3, cols = 4;
    int** matrix = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new int[cols];
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = i * cols + j;
        }
    }

    std::cout << "动态二维数组:" << std::endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << matrix[i][j] << "\t";
        }
        std::cout << std::endl;
    }

    // 释放内存
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

// ==================== 主函数 ====================

int main() {
    SetConsoleOutputCP(CP_UTF8);  // 设置控制台输出为UTF-8
    pointerVsReference();
    smartPointerExamples();
    // weak_ptr 的具体使用场景
    parentChildCircularReference();
    observerPatternExample();
    cacheExample();

    danglingPointerExample();
    pointerArrayVsArrayPointer();
    functionPointerExample();
    referenceParameterExample();
    constPointerExample();
    multiLevelPointerExample();

    return 0;
}