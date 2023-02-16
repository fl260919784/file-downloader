#pragma once

#include <functional>

namespace downloader {
// function可以支持lambda、仿函数以及funtion本身，支持场景更广
using Callable = std::function<void()>;

class Excutor {
  public:
    virtual ~Excutor() = default;
    virtual void push_task(const Callable&) = 0;
    virtual void push_task(Callable &&) = 0;
    virtual bool run() = 0;
};
}  // namespace downloader