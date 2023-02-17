#pragma once

#include <string>

namespace downloader {
class FileDownloader {
  public:
    virtual ~FileDownloader() = default;
    virtual bool download(const std::string &url, const std::string &local_file) = 0;
};
}  // namespace downloader
