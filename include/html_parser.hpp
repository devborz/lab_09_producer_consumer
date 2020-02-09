// Copyright 2020 Usman Turkaev
#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace html {
namespace parser {
std::vector<std::string> find_links(const std::string& html_code) {
  std::vector<std::string> links;
  size_t last_pos = 0;
  while (html_code.find("<a href=\"", last_pos) != std::string::npos) {
    size_t begin = html_code.find("<a href=\"", last_pos);
    begin += 9;
    size_t end = html_code.find("\"", begin);
    std::string link = html_code.substr(begin, end - begin);
    last_pos = end;
    if (link.find("https://") != std::string::npos) {
      links.push_back(link);
    }
  }
  return links;
}

inline bool is_image(const std::string& url) {
  return url.find_last_of(".") != std::string::npos &&
         url.find_last_of(".") > url.find_last_of("/");
}

std::vector<std::string> find_images(const std::string& html_code) {
  std::vector<std::string> images;
  size_t last_pos = 0;
  while (html_code.find("<img src=\"", last_pos) != std::string::npos) {
    size_t begin = html_code.find("<img src=\"", last_pos);
    begin += 10;
    size_t end = html_code.find("\"", begin);
    std::string image = html_code.substr(begin, end - begin);
    last_pos = end;
    if (image.find("https://") != std::string::npos && is_image(image)) {
      images.push_back(image);
    }
  }
  return images;
}
}  // namespace parser

std::string read_html_file(std::string filename = "downloads/page.html") {
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string html_code;
    while (!file.eof()) {
      std::string line;
      std::getline(file, line);
      html_code += line;
    }
    file.close();
    return html_code;
  } else {
    std::logic_error("File with path " + filename + " does not exist");
    return "";
  }
}
}  // namespace html
