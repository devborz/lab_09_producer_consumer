#pragma once
#include <boost/filesystem.hpp>
#include <fstream>
#include <mutex>
#include <string>

namespace fs = boost::filesystem;

class directory_manager {
 public:
  directory_manager() = default;

  directory_manager(const directory_manager &) = delete;

  directory_manager(directory_manager &&) = delete;

  std::string
  create_html_file() {  // const http::response<http::dynamic_body>& /
    // res) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    fs::path path("downloads/pages/");
    size_t number = 0;
    for (const fs::directory_entry &obj : fs::directory_iterator(path)) {
      if (fs::is_regular_file(obj.path())) {
        if (obj.path().extension().string() == ".html" &&
            obj.path().stem().string().substr(0, 5) == "file_") {
          if (std::stoi(obj.path().stem().string().substr(5))) {
            size_t num = std::stoi(obj.path().stem().string().substr(5));
            if (num > number) {
              number = num;
            }
          }
        }
      }
    }
    number++;
    std::string filename =
        "downloads/pages/file_" + std::to_string(number) + ".html";
    // std::ofstream file(filename);
    // if (file.is_open()) {
    //   file << res;
    //   file.close();
    // }
    return filename;
  }

  std::string create_image(const std::string& url) {
    std::lock_guard<std::mutex> lock(this->mutex_);
    fs::path path("downloads/images/");
    size_t number = 0;
    for (const fs::directory_entry &obj : fs::directory_iterator(path)) {
      if (fs::is_regular_file(obj.path())) {
        if (obj.path().extension().string() == ".html" &&
            obj.path().stem().string().substr(0, 5) == "image_") {
          if (std::stoi(obj.path().stem().string().substr(5))) {
            size_t num = std::stoi(obj.path().stem().string().substr(5));
            if (num > number) {
              number = num;
            }
          }
        }
      }
    }
    number++;
    std::string extension;
    std::string filename =
        "downloads/images/image_" + std::to_string(number) + extension;
    return filename;
  }

 private:
  std::mutex mutex_;
};
