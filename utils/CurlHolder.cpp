#include <utils/CurlHolder.h>

namespace downloader {
std::mutex& CurlHolder::curl_easy_init_mutex() {
  // C++11开始，语言规范能够确保局部静态变量的初始化是并发安全的
  static std::mutex curl_easy_init_mutex_;
  return curl_easy_init_mutex_;
}

CurlHolder::CurlHolder() {
  std::lock_guard guard(this->curl_easy_init_mutex());
  // curl_easy_init存在初始化全局数据，需要加锁确保并发安全
  this->handler = curl_easy_init();
}

CurlHolder::CurlHolder(CurlHolder &&rhs) noexcept {
  this->handler = rhs.handler;
  rhs.handler = nullptr;
}

CurlHolder& CurlHolder::operator=(CurlHolder &&rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }
  this->release(handler);
  this->handler = rhs.handler;
  rhs.handler = nullptr;
  return *this;
}

CurlHolder::~CurlHolder() {
  this->release(handler);
}

CURL* CurlHolder::get() const noexcept {
  return this->handler;
}

CurlHolder::operator bool() const noexcept {
  return this->handler != nullptr;
}

void CurlHolder::clear() noexcept {
  this->release(this->handler);
  this->handler = nullptr;
}

void CurlHolder::release(CURL *curl) noexcept {
  if (curl != nullptr) {
    curl_easy_cleanup(curl);
  }
}
}  // namespace downloader
