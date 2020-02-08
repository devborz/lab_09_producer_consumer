// Copyright 2020 Usman Turkaev
#include <gtest/gtest.h>
#include <task.hpp>

using namespace task;

TEST(Example, EmptyTest) {
  std::string url; // = "https://www.adidas.ru";
  std::cout << "Enter url : https://";
  std::cin >> url;
  url = "https://" + url;
  directory_manager manager;
  char curl[url.size() + 1];
  std::strcpy(curl, url.c_str());
  std::string filename = download(curl, manager, "html");
  // std::string html_code = html::read_file(filename);
  // std::vector<std::string> vec = html::parser::find_links(html_code);
  // for (const auto& v: vec) {
  //   std::cerr << v << std::endl;
  // }
  EXPECT_TRUE(true);
}
