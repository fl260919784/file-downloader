# 概要说明

当前程序主要通过

# 概要思路

1. 通过HTTP Response Header中的字段Accept-Ranges确认当前HTTP Server是否支持分段下载
2. 通过HTTP 的HEAD 请求，查询

## dispatcher相关

1. dispatcher通过从thread pool获取task owner，并分派任务
2. thread pool中，若无空闲线程，则可以选择阻塞（如通过ThreadPool::get接口是阻塞）、也支持非阻塞（如通过ThreadPool::try_get）

## 文件写入相关

1. 获取到整体内容的Centent-Length后，创建文件，并resize好
2. 后续每个分段写入到对应的位置即可
3. **好处:**文件不用合并


```c++
using Callable = std::function<bool()>;

class TaskEngine {
  public:
    void push(Callable);

  private:
    
    mutable std::mutex lock;
    mutable std::condition_variable xx; // 生产者与消费者相互通知
    std::list<Callable> tasks;
    std::vector<std::thread> pools;     // 线程创建完成后，通过lock锁来安全的获取task任务，并执行
};
```

# 环境与依赖说明

## 操作系统

ubuntu 18.04

## 编译器

GCC 9.4

## C++标准

C++17

## libcurl

+ 安装说明

```
apt instsall libcurl4-openssl-dev
引入背景： 依赖libcurl发起http等协议请求
补充说明：动态链接没问题，即-lcurl
```

+ version

```
7.68.0
```

## boost

+ 安装说明

```
apt instasll libboost-program-options-dev
引入背景： 解析命令行参数
```

+ version

```
1.71.0
```