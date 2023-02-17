#include <downloader/QuickHTTPFileDownloader.h>

#include <iostream>

namespace downloader {
bool QuickHTTPFileDownloader::set_task_generator(std::unique_ptr<TaskGenerator> &&generator) {
  if (!generator) {
    return false;
  }

  this->generator = std::move(generator);
  return true;
}

bool QuickHTTPFileDownloader::set_task_executor(std::unique_ptr<Executor> &&executor) {
  if (!executor) {
    return false;
  }

  this->executor = std::move(executor);
  return true;
}

bool QuickHTTPFileDownloader::download(const std::string &url, const std::string &local_file) {
  if (!this->executor || !this->generator) {
    return false;
  }

  if (!this->executor->execute(*this->generator)) {
    std::cout << "download: [" << url << "] failed" << std::endl;
    return false;
  }

  return !this->generator->is_error();
}
}  // namespace downloader
