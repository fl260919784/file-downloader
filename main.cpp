#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <downloader/EasyHTTPFileDownloader.h>

namespace {
constexpr const char *version_str = "1.0.0";
std::string url = "http://121.5.146.108";
std::string remote_file = "etcd-cpp-apiv3.tar.gz";
std::string local_file = "download.txt";

int parse(int argc, char *argv[]) noexcept {
    boost::program_options::options_description desc("downloader options");
    auto init = desc.add_options();
    std::string mode;
    init("help,h", "print help and exit");
    init("version,v", "print version and exit");
    init("url", boost::program_options::value<std::string>(&url), "download url");
    init("remote_file", boost::program_options::value<std::string>(&remote_file), "download file");
    init("local_file", boost::program_options::value<std::string>(&local_file), "local file");
    
    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(boost::program_options::parse_command_line((argc>=2 ? 2 : argc), argv, desc), vm);
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

    if (url.empty() || remote_file.empty() || local_file.empty()) {
      std::cout << desc << std::endl;
      return -1;
    }

    return 1;
}
}  // namespace

int main(int argc, char *argv[]) {
  const int rcode = parse(argc, argv);
  if (rcode <= 0) {
      return -rcode;
  }

  downloader::EasyHTTPFileDownloader downloader;
  downloader.set_local_file(local_file);
  downloader.set_url(url);
  if (!downloader.download(remote_file)) {
    printf("download %s failed\n", remote_file.c_str());
    return 1;
  }

  return 0;
}