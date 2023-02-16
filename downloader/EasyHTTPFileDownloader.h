#pragma once

#include <downloader/FileDownloader.h>
#include <utils/CurlHolder.h>

namespace downloader {

class EasyHTTPFileDownloader : public FileDownloader {
  public:
    EasyHTTPFileDownloader() = default;
    EasyHTTPFileDownloader(const EasyHTTPFileDownloader&) = delete;
    EasyHTTPFileDownloader(EasyHTTPFileDownloader &&) noexcept;
    EasyHTTPFileDownloader& operator=(const EasyHTTPFileDownloader&) = delete;
    EasyHTTPFileDownloader& operator=(EasyHTTPFileDownloader &&) noexcept;

  public:
    bool download(const std::string &filename) override;
    void set_url(const std::string &url);
    void set_local_file(const std::string &filename);

  private:
    std::string local_file;
    std::string url;
    CurlHolder curl;
};
}  // namespace downloader
