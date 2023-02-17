#pragma once

#include <functional>

namespace downloader {
// function可以支持lambda、仿函数以及funtion本身，适用场景更广
using Callable = std::function<void()>;

class TaskGenerator {
  public:
    virtual ~TaskGenerator() = default;
    virtual Callable get_task() = 0;  // 返回的Callable布尔判断为false表示无任务
    virtual bool is_error() const = 0;
};

// 设计目标：持续的执行任务，任务执行是否正常由TaskGenerator内部控制
class Executor {
  public:
    virtual ~Executor() = default;
    virtual bool execute(TaskGenerator &) = 0;
};
}  // namespace downloader
