#include <iostream>
#include <memory>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <downloader/EasyHTTPFileDownloader.h>
#include <downloader/QuickHTTPFileDownloader.h>
#include <executor/ThreadPoolExecutor.h>
#include <executor/DownloaderTaskGenerator.h>

namespace {
constexpr const char *version_str = "1.0.0";
std::string url = "http://121.5.146.108/v6.1.tar.gz";
std::string local_file = "linux.v6.1.tar.gz";
int thread_count = 1;
int chunk_size = 1024 * 1024;
bool force_write = false;

int parse(int argc, char *argv[]) noexcept {
  boost::program_options::options_description desc("downloader options");
  auto init = desc.add_options();
  init("help,h", "print help and exit");
  init("version,v", "print version and exit");
  init("url", boost::program_options::value<std::string>(&url), "download url");
  init("output", boost::program_options::value<std::string>(&local_file), "output file");
  init("thread_count", boost::program_options::value<int>(&thread_count), "thread count");
  init("chunk_size", boost::program_options::value<int>(&chunk_size), "download size per task");
  init("force_write", boost::program_options::value<bool>(&force_write), "force write if exists");
  
  boost::program_options::variables_map vm;
  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);   
  } catch(...) {
    std::cout << "parse cmdline failed" << std::endl;
    std::cout << desc << std::endl;
    return -1;
  }
    
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }

  if (vm.count("version")) {
    std::cout << "downloader version: " << version_str << std::endl;
    return 0;
  }

  if (url.empty() || local_file.empty()) {
    std::cout << "paramenter error" << std::endl;
    std::cout << desc << std::endl;
    return -1;
  }
  
  std::cout << "\033[1;33m";            // yellow
  std::cout << "download: [" << url << "], into [" << local_file << "]";
  std::cout << "\033[m" << std::endl;   // reset none
  return 1;
}

std::unique_ptr<downloader::FileDownloader> build_downloader() {
  std::unique_ptr<downloader::QuickHTTPFileDownloader> downloader;

  try {
    auto executor = std::make_unique<downloader::ThreadPoolExecutor>();
    executor->set_thread_count(thread_count);

    downloader::DownloaderTaskGeneratorBuilder builder;
    builder.set_download_url(url);
    builder.set_local_file(local_file);
    builder.set_chunk_size(chunk_size);
    builder.set_force_write(force_write);

    auto generator = builder.build();
    if (!generator) {
      std::cout << "DownloaderTaskGeneratorBuilder failed" << std::endl;
      return downloader;
    }

    downloader = std::make_unique<downloader::QuickHTTPFileDownloader>();
    downloader->set_task_executor(std::move(executor));
    downloader->set_task_generator(std::move(generator));
  } catch (...) {
    downloader.reset();
  }

  return downloader;
}
}  // namespace

int main(int argc, char *argv[]) {
  const int rcode = parse(argc, argv);
  if (rcode <= 0) {
      return -rcode;
  }

  auto downloader = build_downloader();
  if (!downloader) {
    std::cout << "build downloader failed" << std::endl;
    return 1;
  }

  if (!downloader->download(url, local_file)) {
    std::cout << "download [" << url << "] failed" <<std::endl;
    return 1;
  }

  std::cout << "\033[0;32;32m";         // green
  std::cout << "download: [" << url << "] done";
  std::cout << "\033[m" << std::endl;   // reset none
  return 0;
}