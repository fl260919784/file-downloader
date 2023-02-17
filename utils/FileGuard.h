#pragma once

#include <stdio.h>

namespace downloader {
class FileGuard final {
  public:
    explicit FileGuard(FILE *fd) : file(fd) { }
    FileGuard(const FileGuard&) = delete;
    FileGuard(FileGuard &&) noexcept;
    FileGuard& operator=(const FileGuard&) = delete;
    FileGuard& operator=(FileGuard &&) noexcept;
    ~FileGuard();

  public:
    FILE* get() const noexcept {
      return this->file;
    }

    explicit operator bool() const noexcept {
      return file != nullptr;
    }

  private:
    FILE *file = nullptr;
};
}  // namespace downloader
