#include <executor/DownloaderTaskGenerator.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>

#include <iostream>
#include <filesystem>

#include <utils/CurlHolder.h>
#include <utils/FileGuard.h>
#include <utils/FilenoGuard.h>
#include <utils/HTTPRange.h>

namespace downloader {
namespace {
size_t write_data (const void *ptr, size_t size, size_t nmemb, void *file) {
  int fd = reinterpret_cast<long>(file);
  return write(fd, ptr, size*nmemb);
}

bool do_download(const std::string &url,
                 const std::string &local_file,
                 int64_t pos,
                 int64_t chunk_size) {
  try {
    FilenoGuard file(open(local_file.c_str(), O_RDWR));
    if (!file) {
      return false;
    }

    if (lseek64(file.get(), pos, SEEK_SET) == -1) {
      return false;
    }

    CurlHolder curl;
    if (!curl) {
      return false;
    }

    curl_slist *headers = nullptr;
    const auto &range_str = HTTPRange(pos, pos+chunk_size-1).str();
    std::cout << pthread_self() << " -> " << range_str << std::endl;
    headers = curl_slist_append(headers, range_str.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, reinterpret_cast<void*>(file.get()));
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);

    auto res = curl_easy_perform(curl.get());
    if(res != CURLE_OK) {
      std::cout << "curl_easy_perform() failed: "
                << curl_easy_strerror(res) << std::endl;
      return false;
    }
    
    long code = 0;
    // NOTE: curl_easy_getinfo的出参需要是long类型，否则会crash
    res = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &code);
    if(res != CURLE_OK || code < 200 || code >= 300){
      std::cout << "download failed http: ["
                << url << "] response code: " << code << std::endl;
      return false;       
    }
    return true;
  } catch (...) {
    std::cout << "download file failed: exception" << std::endl;
    return false;
  }
}
}  // namespace

DownloaderTaskGenerator::DownloaderTaskGenerator(const DownloaderTaskGeneratorBuilder &builder)
  : total_size(builder.get_total_size()),
    chunk_size(builder.get_chunk_size()),
    url(builder.get_download_url()),
    local_file(builder.get_local_file()) {
  FilenoGuard file(open(local_file.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0644));
  if (!file) {
    std::cout << "open local file error: " << strerror(errno) << std::endl;
    error = true;
  }
}

DownloaderTaskGenerator::~DownloaderTaskGenerator() {
  if (this->error && !this->local_file.empty()) {
    std::cout << "task error, remove " << this->local_file << std::endl;
    unlink(this->local_file.c_str());
  }
}

bool DownloaderTaskGenerator::is_error() const {
  return this->error;
}

Callable DownloaderTaskGenerator::get_task() {
  if (this->error) {
    return nullptr;
  }

  std::lock_guard guard(this->task_lock);
  if (this->downloaded_size >= this->total_size) {
    return nullptr;
  }

  const auto downloaded = this->downloaded_size;
  this->downloaded_size += this->chunk_size;

  auto count_per_task = this->chunk_size;
  if (this->downloaded_size > this->total_size) {
    const auto exceeded = this->total_size - this->downloaded_size;
    count_per_task -= exceeded;
  }

  auto task = [url = this->url,
               local_file = this->local_file,
               pos = downloaded,
               chunk_size = count_per_task,
               this]() {
    if (!do_download(url, local_file, pos, chunk_size)) {
      this->error = true;
    }
  };

  return task;
}

bool DownloaderTaskGeneratorBuilder::set_download_url(const std::string &url_str) {
  if (url_str.empty()) {
    return false;
  }

  this->url = url_str;
  return true;
}

bool DownloaderTaskGeneratorBuilder::set_local_file(const std::string &name) {
  if (name.empty()) {
    return false;
  }

  this->local_file = name;
  return true;
}

bool DownloaderTaskGeneratorBuilder::set_chunk_size(int64_t size) {
  if (size <= 0) {
    return false;
  }

  this->chunk_size = size;
  return true;
}

bool DownloaderTaskGeneratorBuilder::set_force_write(bool force) {
  this->force_write = force;
  return true;
}

int64_t DownloaderTaskGeneratorBuilder::get_total_size() const {
  return this->total_size;
}

int64_t DownloaderTaskGeneratorBuilder::get_chunk_size() const {
  return this->chunk_size;
}

std::string DownloaderTaskGeneratorBuilder::get_download_url() const {
  return this->url;
}

std::string DownloaderTaskGeneratorBuilder::get_local_file() const {
  return this->local_file;
}

int64_t DownloaderTaskGeneratorBuilder::get_file_size() {
  try {
    CurlHolder curl;
    if (!curl) {
      return -1;
    }

    curl_easy_setopt(curl.get(), CURLOPT_URL, this->url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_NOBODY, 1);

    auto res = curl_easy_perform(curl.get());
    if(res != CURLE_OK) {
      return -1;
    }
    
    long code = 0;
    // NOTE: curl_easy_getinfo的出参需要是long类型，否则会crash
    res = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &code);
    if(res != CURLE_OK) {
      return -1;       
    }

    if(code < 200 || code >= 300) {
      std::cout << "get_file_size failed, http: ["
                << this->url << "] response code: " << code << std::endl;
      return -1;       
    }

    curl_off_t file_size = 0;
    res = curl_easy_getinfo(curl.get(), CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &file_size);
    if(res != CURLE_OK){
      return -1;       
    }

    return file_size;
  } catch (...) {
    return -1;
  }
}

std::unique_ptr<DownloaderTaskGenerator> DownloaderTaskGeneratorBuilder::build() noexcept {
  std::unique_ptr<DownloaderTaskGenerator> generator;
  try {
    if (this->chunk_size <= 0) {
      return generator;
    }

    if (this->url.empty()) {
      return generator;
    }

    if (this->local_file.empty()) {
      return generator;
    }

    if (!this->force_write && std::filesystem::exists(this->local_file)) {
      std::cout << "local file: " << this->local_file
                << " exists, force write not allow" << std::endl;
      return generator;
    }

    const auto file_size = this->get_file_size();
    if (file_size <= 0) {
      std::cout << "get download file size failed" << std::endl;
      return generator;
    }

    this->total_size = file_size;
    generator = std::make_unique<DownloaderTaskGenerator>(*this);
  } catch (...) {
    // nothing
  }
  return generator;
}
}  // namespace downloader
