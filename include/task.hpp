// Copyright 2020 Usman Turkaev
#pragma once
#include <gumbo.h>
#include <beast/core.hpp>
#include <beast/http.hpp>
#include <beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <producer_consumer.hpp>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

using namespace boost::program_options;

namespace task {

safe_queue<std::string> find_links(const std::string& url) {}

void download_files(producer_consumer<fs::path>& producer_consumer,
                    safe_queue<std::string>& links) {}

void parse_files(const producer_consumer<fs::path>& producer_consumer_parser,
                 producer_consumer<fs::path>& producer_consumer_printer) {}

void print(const producer_consumer<fs::path>& producer_consumer,
           const fs::path& out) {}

void get_options(variables_map& vm, int argc, char* argv[]) {
  options_description desc("Allowed options");
  desc.add_options()("help", "выводим вспомогательное сообщение")(
      "url", value<std::string>(), "адресс HTML страницы")(
      "depth", value<size_t>(), "глубина поиска по странице")(
      "network_threads", value<size_t>(),
      "количество потоков для скачивания страниц")(
      "parser_threads", value<size_t>(),
      "количество потоков для обработки страниц")("output", value<fs::path>(),
                                                  "путь до выходного файла");

  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);
}

void work(int argc, char* argv[]) {
  variables_map vm;
  get_options(vm, argc, argv);

  std::string url = vm["url"].as<std::string>();

  producer_consumer<fs::path> producer_consumer_parser;
  producer_consumer<fs::path> producer_consumer_printer;

  boost::thread_group network_threads;
  boost::thread_group parser_threads;
  boost::thread_group print_threads;

  safe_queue<std::string> links = find_links(url);

  for (size_t i = 0; i < vm["network_threads"].as<size_t>(); ++i) {
    network_threads.add_thread(new boost::thread(
        download_files, std::ref(producer_consumer_parser), std::ref(links)));
  }
  for (size_t i = 0; i < vm["parser_threads"].as<size_t>(); ++i) {
    parser_threads.add_thread(
        new boost::thread(parse_files, std::ref(producer_consumer_parser),
                          std::ref(producer_consumer_printer)));
  }

  size_t max_num = std::thread::hardware_concurrency();
  fs::path out = vm["out"].as<fs::path>();
  for (size_t i = 0; i < max_num; ++i) {
    print_threads.add_thread(new boost::thread(
        print, std::ref(producer_consumer_printer), std::ref(out)));
  }
  network_threads.join_all();
  parser_threads.join_all();
  print_threads.join_all();
}
}  // namespace task
