#pragma once

#include <stdint.h>
#include <string>

namespace downloader {
class HTTPRange final {
  public:
    HTTPRange(const int64_t first, const int64_t last) {
      resume_from = first;
      finish_at = last;
    }

    std::string str() const {
      std::string from_str =std::to_string(resume_from);
      std::string to_str = std::to_string(finish_at);
      return "Range: bytes=" + from_str + "-" + to_str;
    }

  private:
    int64_t resume_from;
    int64_t finish_at;
};
}  // namespace downloader
