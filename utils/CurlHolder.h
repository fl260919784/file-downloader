#pragma once

#include <curl/curl.h>
#include <mutex>

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
    static std::mutex& curl_easy_init_mutex();

  private:
    CURL *handler = nullptr;
};
}  // namepasce downloader
