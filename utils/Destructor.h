#pragma once

#include <functional>

namespace downloader {

class Destructor final {
  public:
    Destructor(std::function<void()> rhs) : dtor(rhs) {}
    Destructor(const Destructor&) = default;
    Destructor(Destructor &&) = default;
    Destructor& operator=(const Destructor&) = default;
    Destructor& operator=(Destructor&&) = default;
    ~Destructor() {
      if (dtor) {
        dtor();
      }
    }

    void clear() noexcept {
      dtor = nullptr;
    }

  private:
    std::function<void()> dtor;
};
}  // namespace downloader
