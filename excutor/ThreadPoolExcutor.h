#pragma once

#include <excutor/Excutor.h>

#include <mutex>
#include <list>
#include <thread>

namespace downloader {
class ThreadPoolExcutor : public Excutor {
  public:
    void push_task(const Callable&) override;
    void push_task(Callable &&) override;
    bool run() override;

  private:
    mutable std::mutex lock;  // tasks lock
    int thread_count = 0;
    std::list<std::thread> threads;
    std::list<Callable> tasks;
};
}  // namespace downloader
