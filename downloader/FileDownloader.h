#pragma once

#include <string>

namespace downloader {

class FileDownloader {
  public:
    virtual ~FileDownloader() = default;
    virtual bool download(const std::string &filename) = 0;
};
}  // namespace downloader
