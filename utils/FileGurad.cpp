#include <utils/FileGuard.h>

namespace downloader {

FileGuard::FileGuard(FileGuard &&rhs) noexcept {
  this->file = rhs.file;
  rhs.file = nullptr;
}

FileGuard& FileGuard::operator=(FileGuard &&rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  fclose(this->file);
  this->file = rhs.file;
  rhs.file = nullptr;
  return *this;
}

FileGuard::~FileGuard() {
  if (this->file) {
    fclose(this->file);
  }
  this->file = nullptr;
}
}  // namespace downloader
