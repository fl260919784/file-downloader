#pragma once

#include <curl/curl.h>

namespace downloader {

// NOTE: cpr + unique_ptr
class CurlHolder final {
  public:
    CurlHolder();
    CurlHolder(const CurlHolder&) = delete;
    CurlHolder(CurlHolder &&rhs) noexcept;
    CurlHolder& operator=(const CurlHolder&) = delete;
    CurlHolder& operator=(CurlHolder &&rhs) noexcept;
    ~CurlHolder();

  public:
    CURL* get() const noexcept;
    explicit operator bool() const noexcept;
    void clear() noexcept;

  private:
    void release(CURL*) noexcept;

  private:
    CURL *handler = nullptr;
};
}  // namepasce downloader
