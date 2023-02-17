#pragma once

namespace downloader {
class FilenoGuard final {
  public:
    explicit FilenoGuard(int fd) : fileno(fd) { }
    FilenoGuard(const FilenoGuard&) = delete;
    FilenoGuard(FilenoGuard &&) noexcept;
    FilenoGuard& operator=(const FilenoGuard&) = delete;
    FilenoGuard& operator=(FilenoGuard &&) noexcept;
    ~FilenoGuard();

  public:
    int get() const noexcept {
      return this->fileno;
    }

    explicit operator bool() const noexcept {
      return this->fileno >= 0;
    }

    void clear();

  private:
    int fileno = -1;
};
}  // namespace downloader
