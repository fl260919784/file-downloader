#pragma once

#include <executor/Executor.h>

#include <mutex>
#include <memory>
#include <atomic>

namespace downloader {
class DownloaderTaskGeneratorBuilder;
class DownloaderTaskGenerator : public TaskGenerator {
  public:
    explicit DownloaderTaskGenerator(const DownloaderTaskGeneratorBuilder&);
    DownloaderTaskGenerator(const DownloaderTaskGenerator&) = delete;
    DownloaderTaskGenerator(DownloaderTaskGenerator&&) = delete;
    DownloaderTaskGenerator& operator=(const DownloaderTaskGenerator&) = delete;
    DownloaderTaskGenerator& operator=(DownloaderTaskGenerator&&) = delete;
    ~DownloaderTaskGenerator();

  public:
    Callable get_task() override;
    bool is_error() const override;
  
  private:
    mutable std::mutex task_lock;  // 控制并发get_task
    std::atomic<bool> error = false;
    int64_t downloaded_size = 0;
    int64_t total_size = 0;
    int64_t chunk_size = 0;
    std::string url;
    std::string local_file;
};

// XXXBuilder，主要是解决目标类相关属性只允许构造时设置，即不允许构造后变化
// 不使用const修饰数据成员，是为了解决成员属性过多，构造函数需要很多参数，非常丑陋的问题
// 另外XXXBuilder可以在构建目标对象前，进行相关参数校验
// 若代码规范允许构造函数抛出异常，也无需当前XXXBuilder类
class DownloaderTaskGeneratorBuilder final {
  public:
    DownloaderTaskGeneratorBuilder() = default;
    DownloaderTaskGeneratorBuilder(const DownloaderTaskGeneratorBuilder&) = default;
    DownloaderTaskGeneratorBuilder(DownloaderTaskGeneratorBuilder &&) = default;
    DownloaderTaskGeneratorBuilder& operator=(const DownloaderTaskGeneratorBuilder&) = default;
    DownloaderTaskGeneratorBuilder& operator=(DownloaderTaskGeneratorBuilder&&) = default;
    ~DownloaderTaskGeneratorBuilder() = default;

  public:
    std::unique_ptr<DownloaderTaskGenerator> build() noexcept;

  public:
    bool set_download_url(const std::string &url_str);
    bool set_local_file(const std::string &name);
    bool set_chunk_size(int64_t size);
    bool set_force_write(bool force_write);

    int64_t get_total_size() const;
    int64_t get_chunk_size() const;
    std::string get_download_url() const;
    std::string get_local_file() const;

  private:
    int64_t get_file_size();
    static constexpr int kDefaultRangeSizePerTask = 10 * 1024 * 1024;  // 10MB

  private:
    bool force_write = false;
    int64_t total_size = 0;
    int64_t chunk_size = kDefaultRangeSizePerTask;
    std::string url;
    std::string local_file;
};
}  // namespace downloader
