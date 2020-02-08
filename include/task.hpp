// Copyright 2020 Usman Turkaev
#pragma once
#include <algorithm>
// #include <beast/core.hpp>
// #include <beast/http.hpp>
// #include <beast/ssl.hpp>
// #include <beast/version.hpp>
// #include <boost/asio/connect.hpp>
// #include <boost/asio/ip/tcp.hpp>
// #include <boost/asio/ssl/error.hpp>
// #include <boost/asio/ssl/stream.hpp>
// #include <root_certificates.hpp>
#include <curl/curl.h>
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <cstdlib>
#include <cstring>
#include <directory_manager.hpp>
#include <fstream>
#include <html_parser.hpp>
#include <iomanip>
#include <iostream>
#include <producer_consumer.hpp>
#include <string>

using namespace boost::program_options;
// namespace beast = boost::beast; // from <boost/beast.hpp>
// namespace http = beast::http;   // from <boost/beast/http.hpp>
// namespace net = boost::asio;    // from <boost/asio.hpp>
// namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
// using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace fs = boost::filesystem;

namespace task {
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

char* str_to_char(const std::string& str) {
  char* arr = new char[str.size() + 1];
  std::strcpy(arr, str.c_str());
  return arr;
}

std::string download(const char url[], directory_manager& manager,
                     std::string mode) {
  std::string filename;
  if (mode == "html") {
    filename = manager.create_html_file();
  } else if (mode == "img") {
    std::string url_s = std::string(url);
    filename = manager.create_image(url_s);
  }
  char* pagefilename = str_to_char(filename);
  FILE* pagefile;
  CURL* curl_handle;
  curl_global_init(CURL_GLOBAL_ALL);
  /* init the curl session */
  curl_handle = curl_easy_init();
  /* set URL to get here */
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  /* Switch on full protocol/debug output while testing */
  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
  /* disable progress meter, set to 0L to enable it */
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
  /* open the file */
  pagefile = fopen(pagefilename, "wb");
  if (pagefile) {
    /* write the page body to this file handle */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
    /* get it! */
    curl_easy_perform(curl_handle);
    /* close the header file */
    fclose(pagefile);
  }

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();

  delete pagefilename;
  return filename;
}

// std::string download(const std::string& url, directory_manager& manager) {
//   try {
//     auto const host = url;
//     auto const port = "443";
//     auto const target = "/";
//     int version = 11;
//
//     net::io_context ioc;
//     // The SSL context is required, and holds certificates
//     ssl::context ctx(ssl::context::tlsv12_client);
//     // This holds the root certificate used for verification
//     load_root_certificates(ctx);
//     // Verify the remote server's certificate
//     beast::error_code ec0;
//     ctx.set_verify_mode(ssl::verify_peer, ec0);
//     // These objects perform our I/O
//     tcp::resolver resolver(ioc);
//     beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
//     // Set SNI Hostname (many hosts need this to handshake successfully)
//     // if(!SSL_set_tlsext_host_name(stream.native_handle(), host))
//     // {
//     //     beast::error_code ec{static_cast<int>(::ERR_get_error()),
//     net::error::get_ssl_category()};
//     //     throw beast::system_error{ec};
//     // }
//     // Look up the domain name
//     auto const results = resolver.resolve(host, port);
//     // Make the connection on the IP address we get from a lookup
//     beast::get_lowest_layer(stream).connect(results);
//     // Perform the SSL handshake
//     stream.handshake(ssl::stream_base::client);
//     // Set up an HTTP GET request message
//     http::request<http::string_body> req{http::verb::get, target, version};
//     req.set(http::field::host, host);
//     req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
//     // Send the HTTP request to the remote host
//     http::write(stream, req);
//     // This buffer is used for reading and must be persisted
//     beast::flat_buffer buffer;
//     // Declare a container to hold the response
//     http::response<http::dynamic_body> res;
//     // Receive the HTTP response
//     http::read(stream, buffer, res);
//     // Write the message to standard out
//     std::string filename = manager.create_html_file(res);
//     // Gracefully close the stream
//     beast::error_code ec;
//     stream.shutdown(ec);
//     if(ec == net::error::eof) {
//         // Rationale:
//         //
//         //
//         http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
//         ec = {};
//     }
//     return filename;
//   } catch (std::exception const& e) {
//     std::cerr << "Error: " << e.what() << std::endl;
//   }
//   return " ";
// }

std::vector<std::string> find_links(
    const std::string& url, directory_manager& manager,
    producer_consumer<std::string>& producer_consumer) {
  std::vector<std::string> vec;
  char* curl = str_to_char(url);
  std::string filename = download(curl, manager, "html");
  std::string html_code = html::read_file(filename);
  vec = html::parser::find_links(html_code);
  delete curl;
  producer_consumer.produce(filename);
  return vec;
}

void download_files(producer_consumer<std::string>& producer_consumer,
                    std::vector<std::string>::iterator begin,
                    std::vector<std::string>::iterator end, size_t depth,
                    directory_manager& manager) {
  if (depth > 0) {
    for (std::vector<std::string>::iterator it = begin; it < end; ++it) {
      std::vector<std::string> links =
          find_links(*it, manager, producer_consumer);
      download_files(producer_consumer, links.begin(), links.end(), depth - 1,
                     manager);
    }
  }
}

void parse_files(producer_consumer<std::string>& download_parse,
                 producer_consumer<std::string>& parse_print) {
  while (!download_parse.empty()) {
    std::string html_code = html::read_file(download_parse.consume());
    parse_print.produce(html::parser::find_images(html_code));
  }
}

void print(producer_consumer<std::string>& parse_print, //const fs::path& out,
           directory_manager& manager) {
  while (!parse_print.empty()) {
    char* curl = str_to_char(parse_print.consume());
    download(curl, manager, "img");
  }
}

void get_options(variables_map& vm, int argc, char* argv[]) {
  options_description desc("Allowed options");
  desc.add_options()("help", "выводим вспомогательное сообщение")(
      "url", value<std::string>(), "адресс HTML страницы")(
      "depth", value<size_t>(), "глубина поиска по странице")(
      "network_threads", value<size_t>(),
      "количество потоков для скачивания страниц")(
      "parser_threads", value<size_t>(),
      "количество потоков для обработки страниц")("output", value<std::string>(),
                                                  "путь до выходного файла");

  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);
}

void work(int argc, char* argv[]) {
  variables_map vm;
  get_options(vm, argc, argv);

  std::string url = vm["url"].as<std::string>();

  size_t depth = vm["depth"].as<size_t>();

  producer_consumer<std::string> download_parse;
  producer_consumer<std::string> parse_print;

  boost::thread_group network_threads;
  boost::thread_group parser_threads;
  boost::thread_group print_threads;

  size_t network_th_cnt = vm["network_threads"].as<size_t>();
  size_t parser_th_cnt = vm["parser_threads"].as<size_t>();

  directory_manager manager;

  std::vector<std::string> links = find_links(url, manager, download_parse);

  size_t max_num = std::thread::hardware_concurrency();

  // fs::path out = vm["out"].as<fs::path>();

  for (size_t i = 0; i < network_th_cnt; ++i) {
    if (i != network_th_cnt - 1) {
      network_threads.add_thread(new boost::thread(
          download_files, std::ref(download_parse),
          links.begin() + links.size() / network_th_cnt * i,
          links.begin() + links.size() / network_th_cnt * (i + 1), depth,
          std::ref(manager)));
    } else {
      network_threads.add_thread(
          new boost::thread(download_files, std::ref(download_parse),
                            links.begin() + links.size() / network_th_cnt * i,
                            links.end(), depth, std::ref(manager)));
    }
  }
  for (size_t i = 0; i < parser_th_cnt; ++i) {
    parser_threads.add_thread(new boost::thread(
        parse_files, std::ref(download_parse), std::ref(parse_print)));
  }
  for (size_t i = 0; i < max_num; ++i) {
    print_threads.add_thread(
        new boost::thread(print, std::ref(parse_print), //std::ref(out),
      std::ref(manager)));
  }
  network_threads.join_all();
  parser_threads.join_all();
  print_threads.join_all();
}
}  // namespace task
