#pragma once

#include <executor/Executor.h>

#include <list>
#include <thread>

namespace downloader {
class ThreadPoolExecutor : public Executor {
  public:
    ThreadPoolExecutor() = default;
    ThreadPoolExecutor(const ThreadPoolExecutor&) = delete;
    ThreadPoolExecutor(ThreadPoolExecutor&&) noexcept = delete;
    ThreadPoolExecutor& operator=(const ThreadPoolExecutor&) = delete;
    ThreadPoolExecutor& operator=(ThreadPoolExecutor&&) noexcept = delete;
    ~ThreadPoolExecutor() = default;

  public:
    bool execute(TaskGenerator &) override;
    void set_thread_count(int count);

  private:
    void start(TaskGenerator &generator);

  private:
    int thread_count = 1;
    std::list<std::thread> threads;
};
}  // namespace downloader
