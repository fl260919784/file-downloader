#include <downloader/EasyHTTPFileDownloader.h>

#include <unistd.h>
#include <iostream>

#include <utils/FileGuard.h>
#include <utils/CurlHolder.h>
#include <utils/Destructor.h>

namespace downloader {
namespace {
int do_progress(void *clientp, double dltotal,
                double dlnow, double ultotal, double ulnow) {
  static int count = 0;
  ++count;
  if (count % 100 == 0) {
    const int percent = 100.0 * dlnow / dltotal;
    std::cout << "percent: " << percent << "%" << std::endl;
  }
  return 0;
}
}  // namespace

bool EasyHTTPFileDownloader::download(const std::string &url, const std::string &local_file) {
  if (url.empty() || local_file.empty()) {
    return false;
  }

  Destructor cleaner([&local_file]() {
    unlink(local_file.c_str());
  });

  FileGuard file(fopen(local_file.c_str(), "w+"));
  if (!file) {
    cleaner.clear();
    return false;
  }

  try {
    CurlHolder curl;
    if (!curl) {
      std::cout << "download failed: init curl error" << std::endl;
      return false;
    }

    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, file.get());
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl.get(), CURLOPT_PROGRESSFUNCTION, do_progress);
    curl_easy_setopt(curl.get(), CURLOPT_PROGRESSDATA, nullptr);

    auto res = curl_easy_perform(curl.get());
    if(res != CURLE_OK) {
      std::cout << "curl_easy_perform() failed: "
                << curl_easy_strerror(res) << std::endl;
      return false;
    }
    
    long code = 0;
    // NOTE: curl_easy_getinfo的出参需要是long类型，否则会crash
    res = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &code);
    if(res != CURLE_OK || code < 200 || code >= 300) {
      std::cout << "http response error: " << code << std::endl;
      return false;       
    }
  } catch (...) {
    std::cout << "download file failed: exception" << std::endl;
    return false;
  }

  cleaner.clear();
  return true;
}
}  // namespace downloader
