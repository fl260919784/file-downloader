#pragma once 

#include <stdint.h>
#include <memory>

#include <downloader/FileDownloader.h>
#include <executor/Executor.h>

namespace downloader {
class QuickHTTPFileDownloader : public FileDownloader {
  public:
    QuickHTTPFileDownloader() = default;
    QuickHTTPFileDownloader(const QuickHTTPFileDownloader&) = delete;
    QuickHTTPFileDownloader(QuickHTTPFileDownloader &&) noexcept = default;
    QuickHTTPFileDownloader& operator=(const QuickHTTPFileDownloader&) = delete;
    QuickHTTPFileDownloader& operator=(QuickHTTPFileDownloader &&) noexcept = default;
    ~QuickHTTPFileDownloader() = default;

  public:
    bool download(const std::string &url, const std::string &local_file) override;
    bool set_task_generator(std::unique_ptr<TaskGenerator> &&generator);
    bool set_task_executor(std::unique_ptr<Executor> &&executor);

  private:
    std::unique_ptr<TaskGenerator> generator;
    std::unique_ptr<Executor> executor;
};
}  // namespace downloader
