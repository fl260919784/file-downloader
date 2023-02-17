#include <executor/ThreadPoolExecutor.h>

namespace downloader {
void ThreadPoolExecutor::set_thread_count(int size) {
  static const int max_count = std::thread::hardware_concurrency() * 2;
  if (size > max_count) {
    size = max_count;
  }

  this->thread_count = (size <= 0) ? 1 : size;
}

void ThreadPoolExecutor::start(TaskGenerator &generator) {
  for (int count=0; count < this->thread_count; ++count) {
    threads.emplace_back([&generator]() {
      while (true) {
        auto task = generator.get_task();
        if (!task) {
          break;  // all done
        }

        task();
      }
    });
  }
}

bool ThreadPoolExecutor::execute(TaskGenerator &generator) {
  this->start(generator);
  for (auto &t : threads) {
    t.join();
  }
  return true;
}
}  // namespace downlaoder
