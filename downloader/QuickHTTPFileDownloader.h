#pragma once 

#include <downloader/FileDownloader.h>
#include <utils/CurlHolder.h>

namespace downloader {

class QuickHTTPFileDownloader : public FileDownloader {
  public:
    QuickHTTPFileDownloader() = default;
    QuickHTTPFileDownloader(const QuickHTTPFileDownloader&) = delete;
    QuickHTTPFileDownloader(QuickHTTPFileDownloader &&) noexcept;
    QuickHTTPFileDownloader& operator=(const QuickHTTPFileDownloader&) = delete;
    QuickHTTPFileDownloader& operator=(QuickHTTPFileDownloader &&) noexcept;

  public:
    bool download(const std::string &filename) override;
    void set_url(const std::string &url);
    void set_local_file(const std::string &filename);
    void set_thread_count(int size);

  private:
    size_t get_file_size();

  private:
    std::string local_file;
    std::string url;
    CurlHolder curl;
};
}  // namespace downloader
