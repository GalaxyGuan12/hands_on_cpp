
# 基础语法和概念

## **指针和引用**
- 指针和引用的区别？指针可以为空、可以重新赋值，引用必须初始化且不能改变指向
- 智能指针有哪些类型？shared_ptr、unique_ptr、weak_ptr的区别和使用场景
- 什么是野指针？如何避免？

### **指针 vs 引用的关键区别**
- 指针可以为空，引用不能
- 指针可以重新赋值，引用不能改变指向
- 指针需要解引用操作，引用使用更直接

```c++
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
```

### **智能指针的使用场景**
- `unique_ptr`: 独占所有权，不能拷贝只能移动
- `shared_ptr`: 共享所有权，引用计数管理
- `weak_ptr`: 解决循环引用问题

```c++
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
```

```c++
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
```

#### weak_ptr 的三大经典使用场景

##### **解决父子循环引用问题**  
**问题**：父节点持有子节点的shared_ptr，子节点也需要访问父节点，如果子节点也用shared_ptr持有父节点，就会形成循环引用，导致内存泄漏。  
**解决方案**：子节点使用weak_ptr持有父节点引用。

```c++

```

##### 观察者模式
**问题**：Subject需要通知Observer，但不应该控制Observer的生命周期。如果用shared_ptr，Subject会延长Observer的生命周期。
**解决方案**：Subject使用weak_ptr存储Observer列表，Observer可以自由销毁，Subject在通知时检查Observer是否还存在。

##### 缓存系统
**问题**：缓存需要存储对象引用，但不应该阻止对象被垃圾回收。如果用shared_ptr，缓存会永远持有对象。
**解决方案**：缓存使用weak_ptr存储对象，当对象不再被其他地方使用时，自动从缓存中过期。


### **常见陷阱**
- 野指针：未初始化的指针
- 悬空指针：指向已释放内存的指针
- 内存泄漏：忘记释放动态分配的内存

**4. 高级概念**
- 函数指针：用于回调和策略模式
- 常量指针的不同形式
- 多级指针：动态二维数组等应用

### 重点

1. **能够清晰解释概念差异**，不仅知道语法，更要理解底层原理
2. **展示最佳实践**，如使用智能指针管理内存，避免裸指针
3. **识别常见错误**，如内存泄漏、悬空指针等问题
4. **实际应用经验**，结合具体场景说明何时使用指针、何时使用引用

你可以运行这些代码来观察输出结果，加深理解。有什么特定的概念需要我进一步解释吗？

**内存管理**
- new/delete与malloc/free的区别？new会调用构造函数，malloc只分配内存
- 什么是内存泄漏？如何检测和避免？
- 栈和堆的区别？什么时候使用栈，什么时候使用堆？

## 面向对象编程

**继承和多态**
- 虚函数的作用？什么是纯虚函数？
- 虚函数表是什么？虚函数的调用机制？
- 公有继承、保护继承、私有继承的区别？
- 什么是菱形继承问题？如何解决？

**构造函数和析构函数**
- 构造函数可以是虚函数吗？析构函数呢？
- 什么是拷贝构造函数？深拷贝和浅拷贝的区别？
- 移动构造函数的作用？什么是右值引用？
- Rule of Three/Five是什么？

## C++11/14/17新特性

**现代C++特性**
- auto关键字的作用？
- lambda表达式如何使用？
- 什么是范围for循环？
- nullptr与NULL的区别？
- constexpr的作用？

**STL容器**
- vector和list的区别？什么时候使用哪个？
- map和unordered_map的区别？底层实现？
- 如何选择合适的STL容器？

## 高级主题

**模板编程**
- 函数模板和类模板的区别？
- 什么是模板特化？
- SFINAE是什么？
- 可变参数模板如何使用？

**并发编程**
- 如何创建线程？thread类的使用？
- 什么是竞态条件？如何避免？
- mutex、lock_guard的作用？
- 原子操作是什么？atomic类的使用？

## 实际编程问题

**常见算法实现**
- 手写单例模式（线程安全版本）
- 实现一个简单的智能指针
- 写一个线程安全的队列
- 实现RAII类的设计

**性能优化**
- 如何避免不必要的拷贝？
- 内联函数的作用和限制？
- 编译器优化有哪些？如何配合编译器优化？

**调试和错误处理**
- 如何调试内存问题？
- 异常处理的最佳实践？
- 断言的使用场景？

这些问题涵盖了从基础到高级的各个层面。建议你根据自己的经验水平重点准备相应的题目，同时准备一些具体的代码示例来演示你的理解。你想深入了解哪个方面的问题？