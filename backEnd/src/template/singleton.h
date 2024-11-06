//
// Created by 14640 on 2024/11/4.
//

#include <memory>
#include <mutex>

template<typename T>
class Singleton {
public:
    // 获取单例实例
    static T &getInstance() {
        std::call_once(initFlag, &Singleton::initInstance);
        return *instance;
    }

    // 删除拷贝构造函数和赋值运算符
    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton &) = delete;

protected:
    Singleton() = default; // 构造函数保护

private:
    static void initInstance() {
        instance.reset(new T());
    }

    static std::unique_ptr<T> instance; // 存储单例实例
    static std::once_flag initFlag; // 确保初始化只执行一次
};

// 静态成员初始化
template<typename T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;

template<typename T>
std::once_flag Singleton<T>::initFlag;
