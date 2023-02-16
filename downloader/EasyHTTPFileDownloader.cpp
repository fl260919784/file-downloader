#include <downloader/EasyHTTPFileDownloader.h>

#include <unistd.h>

#include <utils/FileGuard.h>
#include <utils/Destructor.h>

namespace downloader {

EasyHTTPFileDownloader::EasyHTTPFileDownloader(EasyHTTPFileDownloader &&rhs) noexcept
  : curl(std::move(rhs.curl)) {
}

EasyHTTPFileDownloader& EasyHTTPFileDownloader::operator=(EasyHTTPFileDownloader &&rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }
  this->curl = std::move(rhs.curl);
  return *this;
}

void EasyHTTPFileDownloader::set_url(const std::string &url) {
  this->url = url;
  if (!url.empty() && url.back() != '/') {
    this->url.append("/");
  }
}

void EasyHTTPFileDownloader::set_local_file(const std::string &filename) {
  this->local_file = filename;
}

bool EasyHTTPFileDownloader::download(const std::string &filename) {
  if (!this->curl || filename.empty() || this->local_file.empty()) {
    return false;
  }

  FileGuard file(fopen(this->local_file.c_str(), "w+"));
  if (!file) {
    return false;
  }

  Destructor cleaner([local_file=this->local_file]() {
    unlink(local_file.c_str());
  });

  try {
    const auto url_str = this->url + filename;
    printf("url: [%s]\n", url_str.c_str());
    curl_easy_setopt(this->curl.get(), CURLOPT_URL, url_str.c_str());
    curl_easy_setopt(this->curl.get(), CURLOPT_WRITEDATA, file.get());

    auto res = curl_easy_perform(this->curl.get());
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      return false;
    }
    
    int httpcode = 0;
    res = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &httpcode);
    if(res != CURLE_OK || httpcode < 200 || httpcode >= 300){
      fprintf(stderr, "httpcode: %d error!\n", httpcode); 
      return false;       
    }
  } catch (...) {
    fprintf(stderr, "download file failed\n");
    return false;
  }

  cleaner.clear();
  return true;
}
}  // namespace downloader
