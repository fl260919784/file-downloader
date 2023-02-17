#include <utils/FilenoGuard.h>

#include <unistd.h>

namespace downloader {
FilenoGuard::FilenoGuard(FilenoGuard &&rhs) noexcept {
  this->fileno = rhs.fileno;
  rhs.fileno = -1;
}

FilenoGuard& FilenoGuard::operator=(FilenoGuard &&rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  close(this->fileno);
  this->fileno = rhs.fileno;
  rhs.fileno = -1;
  return *this;
}

void FilenoGuard::clear() {
  if (this->fileno >= 0) {
    close(this->fileno);
  }
  this->fileno = -1;
}

FilenoGuard::~FilenoGuard() {
  this->clear();
}
}  // namespace downloader
