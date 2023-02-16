#include <utils/CurlHolder.h>

namespace downloader {
  
CurlHolder::CurlHolder()
  : handler(curl_easy_init()) {
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
