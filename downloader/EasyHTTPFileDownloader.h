#pragma once

#include <downloader/FileDownloader.h>

namespace downloader {
class EasyHTTPFileDownloader : public FileDownloader {
  public:
    EasyHTTPFileDownloader() = default;
    EasyHTTPFileDownloader(const EasyHTTPFileDownloader&) = default;
    EasyHTTPFileDownloader(EasyHTTPFileDownloader &&) noexcept = default;
    EasyHTTPFileDownloader& operator=(const EasyHTTPFileDownloader&) = default;
    EasyHTTPFileDownloader& operator=(EasyHTTPFileDownloader &&) noexcept = default;
    ~EasyHTTPFileDownloader() = default;

  public:
    bool download(const std::string &url, const std::string &local_file) override;
};
}  // namespace downloader
